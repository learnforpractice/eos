#include <stdint.h>
#include <array>
#include <vector>
#include <vm_api/vm_api.h>

#include "jmp_stack.h"

typedef std::array<uint8_t, sizeof(jmp_buf)> NlrBuffer;

class JumpStack {
  private:
    std::vector<NlrBuffer> jmp_stack;
    int top;
    int max_stack;

  public:
    JumpStack(size_t _max_stack) {
      max_stack = _max_stack;
      jmp_stack.resize(_max_stack);
      top = -1;
    }

    void push_back(jmp_buf buf) {
      get_vm_api()->eosio_assert(top < max_stack, "stack overflow!");
      top += 1;
      memcpy(jmp_stack[top].data(), buf, sizeof(jmp_buf));
    }

    void pop_back() {
      get_vm_api()->eosio_assert(top >= 0, "stack underflow!");
      top -= 1;
    }

    NlrBuffer& back() {
      get_vm_api()->eosio_assert(top >= 0, "empty stack!");
      return jmp_stack[top];
    }

    void clear() {
      top = -1;
    }
};

static JumpStack setjmp_stack(50);

extern "C" {
#include <stacktrace.h>
void vm_print_stacktrace(void) {
   print_stacktrace();
}

void setjmp_clear_stack() {
  setjmp_stack.clear();
}

void setjmp_push(jmp_buf buf) {
  setjmp_stack.push_back(buf);
}

void setjmp_pop(jmp_buf buf) {
  NlrBuffer& _buf = setjmp_stack.back();
  memcpy(buf, _buf.data(), sizeof(jmp_buf));
  setjmp_stack.pop_back();
}

void setjmp_discard_top() {
  setjmp_stack.pop_back();
}

}

