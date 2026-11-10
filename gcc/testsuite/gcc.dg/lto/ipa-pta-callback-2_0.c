/* { dg-lto-do run } */
/* { dg-lto-options { { -O2 -flto -fipa-pta } } } */

/* fp points to t, which has no IR and calls back into f.  x is only
   reachable from f through c, a second local of g whose address is in the
   static cur, so the call to k clobbers x even though nothing global points
   to x directly.  The indirect call sits in k, so g only learns about the
   callback through the uses and clobbers of k.  */

extern void abort (void);

struct ctx { int pad; int *v; };

static struct ctx *cur;
extern void (*fp) (void);

void
f (void)
{
  *cur->v = 42;
}

__attribute__ ((noinline)) static void
k (void)
{
  fp ();
}

__attribute__ ((noinline)) int
g (void)
{
  int x = 1;
  struct ctx c;

  c.pad = 0;
  c.v = &x;
  cur = &c;
  k ();
  return x;
}

int
main (void)
{
  if (g () != 42)
    abort ();
  return 0;
}
