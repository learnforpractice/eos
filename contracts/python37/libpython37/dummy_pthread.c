#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "eosiolib.h"

int pthread_self(void) {
//    prints(__FUNCTION__);prints("\n");
    return 0;
}

int pthread_cond_destroy(int a) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}

int pthread_cond_init(int a, int b) {
//    prints(__FUNCTION__);prints("\n");
    return 0;
}

int pthread_cond_signal(int a) {
//    prints(__FUNCTION__);prints("\n");
    return 0;
}

int pthread_cond_timedwait(int a, int b, int c) {
    return 0;
}

int pthread_cond_wait(int a, int b) {
    return 0;
}
void pthread_exit(int a) {

}
int pthread_getspecific(int a) {
    return 0;
}
int pthread_key_create(int a, int b) {
    return 0;
}
int pthread_key_delete(int a) {
    return 0;
}
int pthread_mutex_destroy(int a) {
    return 0;
}
int pthread_mutex_init(int a, int b) {
    return 0;
}
int pthread_mutex_lock(int a) {
    return 0;
}
int pthread_mutex_trylock(int a) {
    return 0;
}
int pthread_mutex_unlock(int a) {
    return 0;
}
int pthread_setspecific(int a, int b) {
    return 0;
}

/*
int pthread_attr_init(pthread_attr_t *);
int pthread_attr_setstacksize(pthread_attr_t *, size_t);
int pthread_mutexattr_destroy(pthread_mutexattr_t *);
int pthread_create(pthread_t *__restrict, const pthread_attr_t *__restrict, void *(*)(void *), void *__restrict);
int pthread_detach(pthread_t);
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset);
int pthread_attr_destroy(pthread_attr_t *);
*/

int pthread_attr_destroy(void *a) {
    return 0;
}

int pthread_attr_init(void *a) {
    return 0;
}
int pthread_attr_setstacksize(void * a, size_t b) {
    return 0;
}
int pthread_mutexattr_destroy(void * a) {
    return 0;
}
int pthread_create(void *a, const void *b, void *(*c)(void *), void *d) {
    return 0;
}
int pthread_detach(void *a) {
    return 0;
}

int pthread_sigmask(int how, const void *set, void *oldset) {
    return 0;
}
