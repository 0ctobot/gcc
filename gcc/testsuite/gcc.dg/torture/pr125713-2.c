/* { dg-do run } */
/* { dg-additional-options "-fno-strict-overflow" } */

/* Number of iterations analysis takes a pointer comparison to mean that the
   control variable can't wrap, even under -fwrapv-pointer.  The test on p never
   fires, but its computed iteration count is congruent to 2 modulo the period
   of q, so a test on q would end the loop early.  */

struct s5 { char a[5]; };

__attribute__((noipa)) int
foo (struct s5 *p, struct s5 *e, int *q, unsigned long n)
{
  for (;;)
    {
      p++;
      if (p == e)
	return 1;
      *q++ = 7;
      if (--n == 0)
	return 0;
    }
}

int
main ()
{
  static int a[16];
  static struct s5 b[16];
  __UINTPTR_TYPE__ off = (__UINTPTR_TYPE__) 1 << (sizeof (void *) * 8 - 2);

  if (foo (&b[0], (struct s5 *) ((__UINTPTR_TYPE__) &b[0] + 15 + off), a, 8))
    __builtin_abort ();
  return 0;
}
