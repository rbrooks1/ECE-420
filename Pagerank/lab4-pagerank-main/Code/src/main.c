#define LAB4_EXTEND

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include "../lib/timer.h"
#include "../lib/Lab4_IO.h"

#define EPSILON 0.00001
#define DAMPING_FACTOR 0.85

void
pagerank(double *ranks, struct node *nodes, int node_count, double epsilon, double damping_factor, int comm_sz,
         int mpi_rank, double *latency);

int main() {
    FILE *fp;
    int node_count;
    struct node *nodes;
    double latency;
    double *ranks;
    int comm_sz, mpi_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    // Read problem size and node info from graph metadata file
    // NOTE: We assume every member of the compute cluster can build its own graph
    // from local copies of data_input_meta and data_input_link
    if ((fp = fopen("data_input_meta", "r")) == NULL) {
        perror("fopen");
        return 1;
    }
    fscanf(fp, "%d\n", &node_count);
    fclose(fp);

    // build the graph
    if (node_init(&nodes, 0, node_count) != 0) {
        return 2;
    }

    // compute ranks of each node
    ranks = malloc(node_count * sizeof(double));
    pagerank(ranks, nodes, node_count, EPSILON, DAMPING_FACTOR, comm_sz, mpi_rank, &latency);

    // save ranks and time
    if (mpi_rank == 0) {
        Lab4_saveoutput(ranks, node_count, latency);
    }

    // clean up allocated variables
    free(ranks);
    node_destroy(nodes, node_count);

    MPI_Finalize();
    return 0;
}

/**
 * A parallelized implementation of the PageRank algorithm. Computes the page rank for each node in ranks based on node
 * metadata given in nodes.
 *
 * @param ranks Array of page ranks for each node in nodes.
 * @param nodes An array of nodes representing pages in a directed graph.
 * @param node_count The number of nodes in the graph.
 * @param epsilon The stopping threshold for relative error between current and previous ranks.
 * @param damping_factor A constant in (0,1) representing the probability that the web surfer will continue clicking
 * links in each iteration.
 * @param comm_sz The number of processes running the function.
 * @param mpi_rank The unique rank of the process running the function.
 * @param latency The time it takes for the process to complete the core page rank calculation.
 */
void
pagerank(double *ranks, struct node *nodes, int node_count, double epsilon, double damping_factor, int comm_sz,
         int mpi_rank, double *latency) {
    double start, end;
    double *previous_ranks = malloc(node_count * sizeof(double));
    const double DAMPING_CONSTANT = (1.0 - damping_factor) / node_count; // first term in Eq. 3 of the lab manual
    double *contributions = malloc(node_count * sizeof(double)); // second term in Eq. 3 of the lab manual

    int partition_size = floor(node_count / comm_sz);
    int *PARTITION_SIZES = malloc(comm_sz * sizeof(int));
    const int LOCAL_START = mpi_rank * partition_size;
    int *DISPLACEMENTS = malloc(comm_sz * sizeof(int));
    double *local_ranks;

    // set displacement entries
    MPI_Allgather(&LOCAL_START, 1, MPI_INT, DISPLACEMENTS, 1, MPI_INT, MPI_COMM_WORLD);

    // Augment partition size for the last process (if needed)
    if (mpi_rank == comm_sz - 1) {
        partition_size += node_count % comm_sz;
    }

    // set array of partition sizes
    MPI_Allgather(&partition_size, 1, MPI_INT, PARTITION_SIZES, 1, MPI_INT, MPI_COMM_WORLD);

    local_ranks = calloc(partition_size, sizeof(double));

    // initialize each rank to relative frequency and set initial contributions
    if (mpi_rank == 0) {
        for (int i = 0; i < node_count; ++i) {
            ranks[i] = 1.0 / node_count;
            contributions[i] = damping_factor * ranks[i] / nodes[i].num_out_links;
        }
    }

    GET_TIME(start)

    do {
        // save previous ranks
        if (mpi_rank == 0) {
            memcpy(previous_ranks, ranks, node_count * sizeof(double));
        }

        // share previous ranks with all processes
        MPI_Bcast(previous_ranks, node_count, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // divide current ranks among all processes
        MPI_Scatter(ranks, partition_size, MPI_DOUBLE, local_ranks, partition_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // set the next rank of each node based on the current rank of all outgoing nodes
        for (int i = 0; i < partition_size; ++i) {
            local_ranks[i] = 0;
            for (int j = 0; j < nodes[LOCAL_START + i].num_in_links; ++j) {
                local_ranks[i] += contributions[nodes[LOCAL_START + i].inlinks[j]];
            }
            local_ranks[i] += DAMPING_CONSTANT;
        }

        // update ranks with the local updates from each process
        MPI_Allgatherv(local_ranks, partition_size, MPI_DOUBLE, ranks, PARTITION_SIZES, DISPLACEMENTS, MPI_DOUBLE,
                       MPI_COMM_WORLD);

        // update contributions for each node
        for (int i = 0; i < node_count; ++i) {
            contributions[i] = damping_factor * ranks[i] / nodes[i].num_out_links;
        }
    } while (rel_error(ranks, previous_ranks, node_count) >= epsilon);

    GET_TIME(end)
    *latency = end - start;

    free(previous_ranks);
    free(contributions);
    free(local_ranks);
    free(DISPLACEMENTS);
    free(PARTITION_SIZES);
}
