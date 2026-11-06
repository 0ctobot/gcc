/* PR rtl-optimization/118350 */
/* The unrolled loop's iteration count must survive the runtime count wrapping
   to zero, which happens when the body runs 2^16 times here.  */
/* { dg-do run } */
/* { dg-options "-O2 -funroll-loops -fmodulo-sched" } */

static volatile unsigned long x = 0;
static unsigned short y = 0;

int
main (void)
{
  {
  l:
    ++x;
    --y;
    if (y)
      goto l;
  }
  if (x != 0x10000ul)
    __builtin_abort ();
  return 0;
}
