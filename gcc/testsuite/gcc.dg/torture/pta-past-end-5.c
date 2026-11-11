/* { dg-do run } */
/* { dg-additional-options "-fno-strict-aliasing -fipa-pta" } */

/* With IPA points-to analysis, a pointer one past the end of a local struct
   is moved back by one member in another function, which gives a pointer to
   the last member.  The load through the returned pointer must not be
   computed to read the first member, or x is assumed to point only to l2
   and the store through x is missed.  */

struct pair { int *a; int *b; };

__attribute__((noinline, noclone)) static int **
back (int **p)
{
  return p - 1;
}

__attribute__((noinline, noclone)) static int
f (int c)
{
  int l1 = 0, l2 = 0;
  struct pair s = { &l2, &l1 }, t = { &l2, &l1 };
  int **p = c ? (int **) (&s + 1) : (int **) (&t + 1);
  int *x = *back (p);
  l1 = 1;
  *x = 2;
  return l1;
}

int
main (void)
{
  if (f (1) != 2 || f (0) != 2)
    __builtin_abort ();
  return 0;
}
