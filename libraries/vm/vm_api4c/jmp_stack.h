#ifndef __JMP_STACK_
#define __JMP_STACK_

#include <setjmp.h>

#ifdef __cplusplus
extern "C" {
#endif

void setjmp_clear_stack();
void setjmp_push(jmp_buf buf);
void setjmp_pop(jmp_buf buf);
void setjmp_discard_top();

#ifdef __cplusplus
}
#endif


#endif //__JMP_STACK_