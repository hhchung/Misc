#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>


#ifdef SYS_gettid
#define gettid() syscall(SYS_gettid)
#else
#error "SYS_gettid unavailable on this system"
#endif



//a simple userspace lock
//volatile to prevent from optimization from compiler
volatile int mlock;
int var;
void lock(void){
#ifdef LOCK
    while(__sync_lock_test_and_set(&mlock, 1)) while(mlock);
#endif
}
void unlock(void){
#ifdef LOCK
    __sync_lock_release(&mlock);
#endif
}

void* worker(void *arg){
    int i;
    
    for(i = 0 ; i < 1000000;i++){
        lock();
        var+=1;
        //printf("%ld add var=%d\n",gettid(), var);
        unlock();
    }    
    pthread_exit(NULL);
}



int main(int argc, char *argv[]){
    pthread_t th1, th2;
    var = 0;
    
    pthread_create(&th1, NULL, worker, NULL);
    pthread_create(&th2, NULL, worker, NULL);
    
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    printf("Final %d\n",var);
    if(var != 2000000){
        printf("Lock fail QQ\n");
    }else{
        printf("Lock success\n");

    }

    return 0;
}

