/* { dg-options "-ffixed-x15" } */

/* Keep three values live across an asm that clobbers every call-clobbered
   register but x15, so x15 is the only one left that doesn't need to be
   saved unless it stays fixed.  */

__attribute__ ((noipa)) long
fixed (long a, long b)
{
  long t = a * b, x15;

  __asm__ volatile ("mov x15, %0" : : "r" (0x1515L));
  __asm__ volatile ("" : "+r" (t), "+r" (a), "+r" (b) : :
		    "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8",
		    "x9", "x10", "x11", "x12", "x13", "x14", "x16", "x17",
		    "x18", "x30");
  __asm__ volatile ("mov %0, x15" : "=r" (x15));
  if (x15 != 0x1515)
    __builtin_abort ();
  return t + a + b;
}
