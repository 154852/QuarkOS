.text

.globl _start

_start: # _start is the entry point known to the linker
    xor %ebp, %ebp            # effectively RBP := 0, mark the end of stack frames
    // mov 0(%esp), %edi
    // lea 8(%esp), %esi         # take the address of argv from the stack
    // lea 16(%esp,%edi,8), %edx # take the address of envp from the stack
    xor %eax, %eax            # per ABI and compatibility with icc
    call main                 # %edi, %rsi, %rdx are the three args (of which first two are C standard) to main

    push %eax
    call exit        # terminate the program