#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syscall.h>
#define MSG(fmt, ...) printf( fmt "\n", ##__VA_ARGS__ )




int main(){
    long long value;
    long long addr;
    long long scno;
    char buf[50] = "This test\n"; 
    value = 0;
    scno = __NR_getpid; 
    MSG("PID = %d by libc call", getpid());
    asm volatile(
        "mov x0, #0    \n\t"        
        "mov x8, #172    \n\t" //syscall call register number x8
        "svc #0        \n\t"
        "mov %0, x0    \n\t"
    :"=r"(value) : "r"(scno) : "memory");
    //ams(code:output operand list:input operand list: clobber) 
    //=:write only
    //+:read only
    //&:output only
    //r: general register
    //$0:starts from output operand
    //MSG("str = %s", buf);
    MSG("PID = %lld by asm\n", value);
    //MSG("%s", strerror(value));
    return 0;
}

