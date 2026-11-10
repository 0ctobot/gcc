/* { dg-lto-do run } */
/* { dg-lto-options { { -O2 -flto -fipa-pta } } } */

/* h has no IR, so the call to it leaves the partition, and h calls back
   into f, which stores to x through the static sp.  x never escapes since
   nothing outside of the partition can read sp, so only the uses and
   clobbers of f tell that the call to h clobbers x.  */

extern void abort (void);
extern void h (void);

static int *sp;

__attribute__ ((noinline)) void
f (void)
{
  *sp = 42;
}

__attribute__ ((noinline)) int
g (void)
{
  int x = 1;

  sp = &x;
  h ();
  return x;
}

int
main (void)
{
  if (g () != 42)
    abort ();
  return 0;
}
