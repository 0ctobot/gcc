/* Instructions in the function's own section still count toward the size
   of an asm when it also writes to another section, so this helper stays
   too large for the early inliner.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-einline-details --param early-inlining-insns=2" } */

static inline unsigned long
f_long (void)
{
  unsigned long x;
  __asm__ ("1:\tnop %0\n"
	   "\tnop %0\n"
	   "\tnop %0\n"
	   "\tnop %0\n"
	   "\tnop %0\n"
	   "\tnop %0\n"
	   "\tnop %0\n"
	   "\tnop %0\n"
	   "\t.pushsection .patch_table, \"a\"\n"
	   "\t.long 1b - .\n"
	   "\t.popsection\n"
	   : "=r" (x));
  return x;
}

unsigned long g1 (void) { return f_long () + 1; }
unsigned long g2 (void) { return f_long () + 2; }

/* { dg-final { scan-tree-dump-times "will not early inline: \[^\n\]*f_long" 2 "einline" } } */
/* { dg-final { scan-tree-dump-not "Inlining f_long" "einline" } } */
