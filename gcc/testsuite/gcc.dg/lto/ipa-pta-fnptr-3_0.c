/* { dg-lto-do run } */
/* { dg-lto-options { { -O2 -flto -fipa-pta } } } */

/* The targets of the op member have no IR and are reached only through static
   initializers.  The pointer they return must be able to point to x.  */

extern void abort (void);
extern int *ext1 (void);
extern int *ext2 (void);

struct ops
{
  int *(*op) (void);
};

static const struct ops o1 = { .op = ext1 }, o2 = { .op = ext2 };

int x;
volatile int sel;

__attribute__ ((noinline)) static int
f (const struct ops *o)
{
  x = 1;
  int *r = o->op ();
  *r = 2;
  return x;
}

int
main (void)
{
  if (f (sel ? &o1 : &o2) != 2)
    abort ();
  sel = 1;
  if (f (sel ? &o1 : &o2) != 2)
    abort ();
  return 0;
}
