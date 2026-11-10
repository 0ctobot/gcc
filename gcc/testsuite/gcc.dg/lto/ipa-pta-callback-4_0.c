/* { dg-lto-do run } */
/* { dg-lto-options { { -O2 -flto -fipa-pta -fno-ipa-cp } } } */

/* Like ipa-pta-callback-2_0.c, but the callback f doesn't store to x
   itself: it calls w through a static table, and w does.  g reaches the
   indirect call in k through a function pointer passed to m, which
   -fno-ipa-cp keeps from becoming a direct call to k.  */

extern void abort (void);

static int *sp;

__attribute__ ((noinline)) static void
w (void)
{
  *sp = 42;
}

__attribute__ ((noinline)) static void
n (void)
{
}

static void (*const tbl[2]) (void) = { n, w };
int sel = 1;
extern void (*fp) (void);

void
f (void)
{
  tbl[sel & 1] ();
}

__attribute__ ((noinline)) static void
k (void)
{
  fp ();
}

__attribute__ ((noinline)) static void
m (void (*fn) (void))
{
  fn ();
}

__attribute__ ((noinline)) int
g (void)
{
  int x = 1;

  sp = &x;
  m (k);
  return x;
}

int
main (void)
{
  if (g () != 42)
    abort ();
  return 0;
}
