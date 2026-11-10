/* { dg-lto-do run } */
/* { dg-lto-options { { -O2 -flto -fipa-pta } } } */

/* Like ipa-pta-callback-2_0.c, but the callback only reads x, so the
   store of 41 ahead of the first call to k isn't dead.  */

extern void abort (void);

static int *sp;
int seen;
extern void (*fp) (void);

void
f (void)
{
  seen += *sp;
}

__attribute__ ((noinline)) static void
k (void)
{
  fp ();
}

__attribute__ ((noinline)) int
g (void)
{
  int x;

  sp = &x;
  x = 41;
  k ();
  x = 1;
  k ();
  sp = 0;
  return seen;
}

int
main (void)
{
  if (g () != 42)
    abort ();
  return 0;
}
