/* { dg-options "-ffixed-x15" } */

/* Keep three values live across an asm that clobbers every call-clobbered
   register but x15 and x18, so those are the only two left that don't need
   to be saved.  */

__attribute__ ((noipa)) long
many (long a, long b)
{
  long t = a * b;
  __asm__ volatile ("" : "+r" (t), "+r" (a), "+r" (b) : :
		    "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8",
		    "x9", "x10", "x11", "x12", "x13", "x14", "x16", "x17",
		    "x30");
  return t + a + b;
}
