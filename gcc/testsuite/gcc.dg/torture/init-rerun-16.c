/* { dg-do run } */
/* { dg-require-effective-target int32plus } */
/* { dg-options "-ftrivial-auto-var-init=zero" } */

/* An initializer that a jump back runs again gets built in a temporary,
   and the first run reads the variable before anything stored to it.  With
   automatic variable initialization, that read has to see the zeroes the
   variable starts with, as it does when the initializer is built in place.
   The second run reads the value of the first, and a new entry to the block
   of the variable starts it over from zero.  */

struct S { int a[4]; int k; };

__attribute__ ((noipa)) void
clobber (void)
{
  volatile char b[256];
  for (int i = 0; i < 256; i++)
    b[i] = 0x5a;
}

__attribute__ ((noipa)) int
by_name (int n)
{
  int res = 0, runs = 0;
 again:;
  struct S v = { .k = 1, .a[0] = runs ? v.k : v.k + 100, .a[1] = 2 };
  res = res * 1000 + v.a[0];
  if (++runs < n)
    goto again;
  return res;
}

__attribute__ ((noipa)) int
in_block (int n)
{
  int res = 0;
  for (int i = 0; i < n; i++)
    {
      int runs = 0;
    again:;
      struct S v = { .k = 7, .a[0] = v.k + v.a[1] + 1, .a[1] = 4 };
      res = res * 100 + v.a[0];
      if (++runs < 2)
	goto again;
    }
  return res;
}

int
main (void)
{
  clobber ();
  if (by_name (2) != 100001)
    __builtin_abort ();
  clobber ();
  if (in_block (2) != 1120112)
    __builtin_abort ();
  return 0;
}
