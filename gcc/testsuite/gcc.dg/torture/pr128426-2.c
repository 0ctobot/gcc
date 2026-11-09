/* { dg-do compile } */
/* { dg-require-effective-target indirect_jumps } */
/* { dg-require-effective-target label_values } */

/* Besides Q, the only use of P is its own argument on the abnormal edge from
   the computed goto.  Folding the condition of __builtin_unreachable leaves
   both dead.  No-delete DCE zeroes the other arguments of P but has to keep
   the abnormal one, and that use of P by itself used to queue P again
   forever.  */

int a;

int
f (int y)
{
  static void *t[] = { &&b, &&out };
  int p = y;
  if (a)
    p = 5;
b:
  a++;
  goto *t[a & 1];
out:;
  int q = a > 3 ? p : 0;
  if (q != 0)
    __builtin_unreachable ();
  return 0;
}
