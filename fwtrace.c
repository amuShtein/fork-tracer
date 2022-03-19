#define _GNU_SOURCE
#include <dlfcn.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef pid_t (*fork_t) (void);
typedef pid_t (*wait_t) (int* );
typedef pid_t (*waitpid_t) (pid_t, int* , int);

pid_t mainpid = -1;
int cnt = 0;

pid_t fork(void) {
    fflush(stdout);
    if(mainpid == -1) {
        printf("[main pid = %d]\n", getpid());
        mainpid = getpid();
    }

    cnt++;

    fork_t original_fork = (fork_t) dlsym(RTLD_NEXT, "fork");
    // printf("forking... [%ld] at %d\n", time(0), getpid());
    pid_t pid = original_fork();

    if(pid == 0) {
        printf("[forked] pid = %d, ppid = %d\n", getpid(), getppid());
    } else if (pid == -1) {
        printf("[fork failed] at %d\n", getpid());
    }

    fflush(stdout);
    return pid;
}

pid_t wait(int *wstatus) {
    wait_t original_wait = dlsym(RTLD_NEXT, "wait");
    fflush(stdin);
    // printf("waiting... [%ld] at %d\n", time(0), getpid());
    pid_t pid = original_wait(wstatus);
    fflush(stdout);

    if(pid == -1) {
        printf("[wait failed]\n");
    } else {
        printf("[catched] %d from %d\n", pid, getpid());
        cnt--;
    }


    if(cnt == 0 && getpid() == mainpid) {
        printf("[All child processes catched]\n");
    }

    fflush(stdout);
    return pid;
}

pid_t waitpid(pid_t pid, int *wstatus, int options) {
    waitpid_t original_waitpid = dlsym(RTLD_NEXT, "waitpid");
    fflush(stdin);
    // printf("waiting... [%ld] at %d\n", time(0), getpid());
    pid_t ret_pid = original_waitpid(pid, wstatus, options);
    fflush(stdout);

    if(pid == -1) {
        printf("[waitpid %d failed]\n", pid);
    } else {
        printf("[catched] %d from %d\n", ret_pid, getpid());
        cnt--;
    }

    if(cnt == 0 && getpid() == mainpid) {
        printf("[All child processes catched]\n");
    }

    fflush(stdout);
    return pid;
}