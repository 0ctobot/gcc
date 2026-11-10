/* { dg-options "-fwrapv" } */

int sink;

static __attribute__ ((noinline)) int
g (int y)
{
  sink++;
  return y > 0 ? 1 : 2;
}

static __attribute__ ((noinline)) int
f (int x)
{
  sink += 2;
  return g (x + 1);
}

int h1 (void) { return f (5); }
int h2 (void) { return f (__INT_MAX__); }	/* g (INT_MIN) returns 2.  */
int h3 (void) { return f (77); }
