/* PR target/124266 */
/* { dg-do run } */
/* { dg-options "-O2 -fasynchronous-unwind-tables" } */

/* f1 only saves D15 on one path through it, and pair fusion must not lose the
   CFI that ends the save.  Otherwise, the code after the if-else claims that
   the caller's D15 lives in f1's save slot, even when f1 took the path that
   doesn't write to the slot.  Unwinding from there has to find the value that
   the caller put in D15.  */

#include <unwind.h>

#define VPCS __attribute__ ((aarch64_vector_pcs))
#define NOIPA __attribute__ ((noipa))
#define MAGIC 0x1122334455667788UL

extern void abort (void);

volatile int x;
static int seen;

NOIPA void caller (void);

static _Unwind_Reason_Code
callback (struct _Unwind_Context *ctx, void *data)
{
  if (_Unwind_GetRegionStart (ctx) != (_Unwind_Ptr) caller)
    return _URC_NO_REASON;

  if (_Unwind_GetGR (ctx, 79) != MAGIC)
    abort ();
  seen = 1;
  return _URC_NORMAL_STOP;
}

VPCS NOIPA void
check (void)
{
  _Unwind_Backtrace (callback, 0);
}

VPCS NOIPA void
callee (void)
{
}

/* The calls after the if-else keep the join from being duplicated into its
   predecessors, and __builtin_expect makes dwarf2cfi see the path that saves
   D15 first.  */
VPCS NOIPA int
f1 (void)
{
  if (__builtin_expect (x, 1))
    {
      asm ("" : : : "v15", "v16");
      callee ();
    }
  else
    callee ();
  check ();
  callee ();
  callee ();
  callee ();
  callee ();
  callee ();
  callee ();
  callee ();
  return 0;
}

/* Fill the part of the stack that f1 is about to use.  */
NOIPA void
dirty (void)
{
  volatile char buf[1024];
  for (int i = 0; i < 1024; i++)
    buf[i] = 0xaa;
}

NOIPA void
caller (void)
{
  dirty ();
  asm volatile ("fmov d15, %0" : : "r" (MAGIC) : "d15");
  f1 ();
  asm volatile ("");
}

int
main (void)
{
  caller ();
  if (!seen)
    abort ();
  return 0;
}
