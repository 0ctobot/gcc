/* Code hoisting places an insn at the end of the common dominator, which must
   not put a division that can trap above a volatile asm goto ending that block,
   since the asm's effect has to be seen before the division faults.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-rtl-hoist-details" } */

extern void g (int);

__attribute__ ((cold)) int
f (int c, int x, int y, int d)
{
  int r;
  if (c)
    goto a;
  asm goto ("cbz %w0, %l1" : : "r" (d) : : b);
  if (d)
    {
      r = x / y;
      g (r);
      return r - 1;
    }
  goto a;
b:
  r = x / y;
  g (r);
  return r + 5;
a:
  r = x / y;
  g (r);
  return r * 3;
}

/* { dg-final { scan-rtl-dump-not "copying expression" "hoist" } } */
