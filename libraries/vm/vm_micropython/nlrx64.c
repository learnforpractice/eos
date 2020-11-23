/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


// x86-64 callee-save registers are:
//  rbx, rbp, rsp, r12, r13, r14, r15
#include <setjmp.h>
#include <stdint.h>

typedef jmp_buf nlr_buf_t;

void setjmp_push(jmp_buf buf);
void setjmp_pop(jmp_buf buf);
void *get_memory_ptr(uint32_t offset, uint32_t size);

uint32_t mp_nlr_jmp_head(uint32_t val);
uint32_t nlr_push_tail(uint32_t nlr_offset);

__attribute__((used)) unsigned int vm_nlr_push_tail(nlr_buf_t *nlr) {
    setjmp_push(*nlr);
    return 0; //normal return
}

unsigned int vm_nlr_push(nlr_buf_t *nlr) {

    #if MICROPY_NLR_OS_WINDOWS

    __asm volatile (
        "movq   (%rsp), %rax        \n" // load return %rip
        "movq   %rax, 16(%rcx)      \n" // store %rip into nlr_buf
        "movq   %rbp, 24(%rcx)      \n" // store %rbp into nlr_buf
        "movq   %rsp, 32(%rcx)      \n" // store %rsp into nlr_buf
        "movq   %rbx, 40(%rcx)      \n" // store %rbx into nlr_buf
        "movq   %r12, 48(%rcx)      \n" // store %r12 into nlr_buf
        "movq   %r13, 56(%rcx)      \n" // store %r13 into nlr_buf
        "movq   %r14, 64(%rcx)      \n" // store %r14 into nlr_buf
        "movq   %r15, 72(%rcx)      \n" // store %r15 into nlr_buf
        "movq   %rdi, 80(%rcx)      \n" // store %rdr into nlr_buf
        "movq   %rsi, 88(%rcx)      \n" // store %rsi into nlr_buf
        "jmp    vm_nlr_push_tail       \n" // do the rest in C
        );

    #else

    __asm volatile (
        #if defined(__APPLE__) || defined(__MACH__)
        "pop    %rbp                \n" // undo function's prelude
        #endif
        "movq   (%rsp), %rax        \n" // load return %rip
        "movq   %rax, 16(%rdi)      \n" // store %rip into nlr_buf
        "movq   %rbp, 24(%rdi)      \n" // store %rbp into nlr_buf
        "movq   %rsp, 32(%rdi)      \n" // store %rsp into nlr_buf
        "movq   %rbx, 40(%rdi)      \n" // store %rbx into nlr_buf
        "movq   %r12, 48(%rdi)      \n" // store %r12 into nlr_buf
        "movq   %r13, 56(%rdi)      \n" // store %r13 into nlr_buf
        "movq   %r14, 64(%rdi)      \n" // store %r14 into nlr_buf
        "movq   %r15, 72(%rdi)      \n" // store %r15 into nlr_buf
        #if defined(__APPLE__) || defined(__MACH__)
        "jmp    _vm_nlr_push_tail      \n" // do the rest in C
        #else
        "jmp    vm_nlr_push_tail       \n" // do the rest in C
        #endif
        );

    #endif

    return 0; // needed to silence compiler warning
}

nlr_buf_t *vm_nlr_jmp_head(uint32_t val) {
    uint32_t top_offset = mp_nlr_jmp_head(val);
    return get_memory_ptr(top_offset, sizeof(nlr_buf_t));
}

static nlr_buf_t g_top;
__attribute__((noreturn)) void vm_nlr_jump(uint32_t val) {
//    nlr_buf_t *top = vm_nlr_jmp_head(val);
    nlr_buf_t *top = &g_top;
    setjmp_pop(*top);

    __asm volatile (
        "movq   %0, %%rcx           \n" // %rcx points to nlr_buf
        #if MICROPY_NLR_OS_WINDOWS
        "movq   88(%%rcx), %%rsi    \n" // load saved %rsi
        "movq   80(%%rcx), %%rdi    \n" // load saved %rdr
        #endif
        "movq   72(%%rcx), %%r15    \n" // load saved %r15
        "movq   64(%%rcx), %%r14    \n" // load saved %r14
        "movq   56(%%rcx), %%r13    \n" // load saved %r13
        "movq   48(%%rcx), %%r12    \n" // load saved %r12
        "movq   40(%%rcx), %%rbx    \n" // load saved %rbx
        "movq   32(%%rcx), %%rsp    \n" // load saved %rsp
        "movq   24(%%rcx), %%rbp    \n" // load saved %rbp
        "movq   16(%%rcx), %%rax    \n" // load saved %rip
        "movq   %%rax, (%%rsp)      \n" // store saved %rip to stack
        "xorq   %%rax, %%rax        \n" // clear return register
        "inc    %%al                \n" // increase to make 1, non-local return
        "ret                        \n" // return
        :                           // output operands
        : "r" (top)                 // input operands
        :                           // clobbered registers
        );

    for(;;);
}
