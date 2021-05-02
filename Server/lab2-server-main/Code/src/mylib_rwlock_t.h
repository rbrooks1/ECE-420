/*
 * This read-write lock implementation is based on sample code provided by Dr. Di Niu from Chapter 1 of his Parallel and
 * Distributed Programming lectures given in Winter 2021 at the University of Alberta.
 */
#ifndef LAB2_SERVER_MYLIB_RWLOCK_T_H
#define LAB2_SERVER_MYLIB_RWLOCK_T_H

#include <pthread.h>

typedef struct {
    int readers;
    int writer;
    pthread_cond_t readers_proceed;
    pthread_cond_t writer_proceed;
    int pending_writers;
    pthread_mutex_t read_write_lock;
} mylib_rwlock_t;

void mylib_rwlock_init(mylib_rwlock_t *l);

void mylib_rwlock_rlock(mylib_rwlock_t *l);

void mylib_rwlock_wlock(mylib_rwlock_t *l);

void mylib_rwlock_unlock(mylib_rwlock_t *l);

#endif //LAB2_SERVER_MYLIB_RWLOCK_T_H
