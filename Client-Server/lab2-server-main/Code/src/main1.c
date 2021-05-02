#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include <stdbool.h>
#include "../lib/common.h"
#include "../lib/timer.h"

#define EXPECTED_ARGC 4

char **strings;
pthread_mutex_t strings_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_request(void *args);

/*
 * Program entry point.
 *
 * This program launches a server that uses multithreading to support simultaneous client connections. Clients may read
 * or write to a global array of strings which is guarded by a single mutex.
 *
 * When running main1, three parameters are expected for string array length, server IP address, and server port,
 * respectively. The server assumes that 1000 client requests will be sent at a time, which is the default value of
 * COM_NUM_REQUEST set in common.h. Make sure the client application is compiled with the same common.h to prevent
 * issues at runtime.
 */
int main(int argc, char *argv[]) {
    if (argc != EXPECTED_ARGC) {
        fprintf(stderr,
                "3 parameters must be provided for string array length, IP address, and port, respectively.\n");
        return 1;
    }

    struct sockaddr_in server_socket;
    int server_fd, client_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return 1;
    }
    pthread_t threads[COM_NUM_REQUEST];

    int num_strings = atoi(argv[1]);
    strings = (char **) malloc(num_strings * sizeof(char *));
    for (int i = 0; i < num_strings; ++i) {
        strings[i] = (char *) malloc(COM_BUFF_SIZE * sizeof(char));
        sprintf(strings[i], "String %d: the initial value", i);
    }

    double times[COM_NUM_REQUEST] = {0};

    server_socket.sin_addr.s_addr = inet_addr(argv[2]);
    server_socket.sin_port = atoi(argv[3]);
    server_socket.sin_family = AF_INET;

    if (bind(server_fd, (struct sockaddr *) &server_socket, sizeof(server_socket)) == 0) {
        printf("server listening on %s:%s\n", argv[2], argv[3]);
        listen(server_fd, COM_NUM_REQUEST);
        while (true) {
            for (int i = 0; i < COM_NUM_REQUEST; ++i) {
                client_fd = accept(server_fd, NULL, NULL);
                pthread_create(&threads[i], NULL, handle_request, (void *) (long) client_fd);
            }
            for (int i = 0; i < COM_NUM_REQUEST; ++i) {
                double *requestTime;
                pthread_join(threads[i], (void **) &requestTime);
                times[i] = *requestTime;
                free(requestTime);
            }
            saveTimes(times, COM_NUM_REQUEST);
        }
        close(server_fd); // will never be reached
    } else {
        perror("bind");
        return 1;
    }

    return 0;
}

/*
 * Thread worker function that handles a single client request (read or write).
 *
 * The args parameter is assumed to contain a file descriptor to the client socket, which contains a ClientRequest to be
 * parsed. The read or write request is performed by the server, and the given string element is sent by the server as a
 * response back to the client.
 *
 * A single mutex is used to synchronize access to the string array.
 */
void *handle_request(void *args) {
    int client_fd = (int) (long) args;
    char msg[COM_BUFF_SIZE], server_msg[COM_BUFF_SIZE];
    double start, end;
    double *time_delta = malloc(sizeof(double));
    ClientRequest client_request;
    read(client_fd, msg, COM_BUFF_SIZE);

    // fill up ClientRequest and modify string array
    ParseMsg(msg, &client_request);

    GET_TIME(start)
    if (client_request.is_read) {
        pthread_mutex_lock(&strings_mutex);
        getContent(server_msg, client_request.pos, strings);
        pthread_mutex_unlock(&strings_mutex);
    } else { // Assume write operation
        pthread_mutex_lock(&strings_mutex);
        setContent(client_request.msg, client_request.pos, strings);
        getContent(server_msg, client_request.pos, strings);
        pthread_mutex_unlock(&strings_mutex);
    }
    GET_TIME(end)
    *time_delta = end - start;

    write(client_fd, server_msg, COM_BUFF_SIZE);
    close(client_fd);

    pthread_exit(time_delta);
}