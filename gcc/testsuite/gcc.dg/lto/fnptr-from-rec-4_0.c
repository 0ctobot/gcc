/* { dg-lto-do link } */
/* { dg-lto-options { { -O2 -flto -fdump-ipa-devirt } } } */

/* Each unit stores one function to one field of struct ops and two different
   functions to the other, so neither field has a single target in the
   program.  The conflicts have to survive streaming in both orders: the
   single store of a is read before the conflict and the single store of b
   after it.  */

struct ops
{
  int (*a) (int);
  int (*b) (int);
};

struct obj
{
  const struct ops *ops;
};

extern struct obj o1, o2;

int __attribute__ ((noinline))
call_a (struct obj *o, int x)
{
  return o->ops->a (x);
}

int __attribute__ ((noinline))
call_b (struct obj *o, int x)
{
  return o->ops->b (x);
}

/* Calls in a loop, so the calls through the fields aren't cold.  */

volatile int count = 1000;

int
main (void)
{
  int r = 0;
  for (int i = 0; i < count; i++)
    r += call_a (&o1, i) + call_b (&o2, i) + call_a (&o2, i) + call_b (&o1, i);
  return r == 0;
}

/* { dg-final { scan-wpa-ipa-dump-not "-> speculative call" "devirt" } } */
