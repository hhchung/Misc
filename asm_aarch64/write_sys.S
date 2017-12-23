#include <asm-generic/unistd.h>
.balign 8
.data
msg:
	.ascii "testing \n"
len = . -msg


.text

	.global _start
_start:
	mov x8, #__NR_write
	mov x0, #0
	ldr x1, =msg
	ldr x2, =len 
	svc #0
    	mov x8, #__NR_exit_group
	mov x0, #97
	svc #0

