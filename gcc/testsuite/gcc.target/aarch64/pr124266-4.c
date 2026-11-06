/* PR target/124266 */
/* { dg-do run } */
/* { dg-options "-O2 -fomit-frame-pointer -fasynchronous-unwind-tables -save-temps" } */

/* g saves LR right next to its last outgoing stack argument, and pair fusion
   merges the two stores.  The CFI for the save of LR needs to survive that,
   since the unwinder otherwise thinks that LR still holds g's return address
   during the call to f and cannot get past g.  */

#include <unwind.h>

#define NOIPA __attribute__ ((noipa))

extern void abort (void);

int main (void);

static int frames, seen;

static _Unwind_Reason_Code
callback (struct _Unwind_Context *ctx, void *data)
{
  if (_Unwind_GetRegionStart (ctx) == (_Unwind_Ptr) main)
    {
      seen = 1;
      return _URC_NORMAL_STOP;
    }

  if (++frames > 8)
    abort ();
  return _URC_NO_REASON;
}

NOIPA void
f (double a0, double a1, double a2, double a3, double a4, double a5,
   double a6, double a7, double a8, double a9, double a10, double a11)
{
  _Unwind_Backtrace (callback, 0);
}

NOIPA void
h (void)
{
}

/* The final argument is 0 in order to store it from XZR, which gives it a
   different register class from the other stack arguments.  That leaves it
   to pair up with LR.  */
NOIPA void
g (void)
{
  f (1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0);
  h ();
}

int
main (void)
{
  g ();
  if (!seen)
    abort ();
  return 0;
}

/* { dg-final { scan-assembler {stp\txzr, x30, \[sp, 24\]} } } */
/* { dg-final { scan-assembler {\.cfi_offset 30, -16\n\tbl\tf\n} } } */
