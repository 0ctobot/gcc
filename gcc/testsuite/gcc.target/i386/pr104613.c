/* PR target/104613 */
/* { dg-do compile { target ia32 } } */
/* { dg-options "-O2 -fno-omit-frame-pointer -mfunction-return=thunk" } */

struct _Unwind_Context _Unwind_Resume_or_Rethrow_this_context;

void offset (int);

struct _Unwind_Context {
  void *reg[7];
} _Unwind_Resume_or_Rethrow (void)
{
  struct _Unwind_Context cur_contextcur_context
    = _Unwind_Resume_or_Rethrow_this_context;
  offset (0);
  __builtin_eh_return ((__INTPTR_TYPE__) offset, 0);
}

/* { dg-final { scan-assembler "jmp\[ \t\]+_*x86_return_thunk_ecx|jmp\[ \t\]+_*x86_indirect_thunk_ecx" } } */
