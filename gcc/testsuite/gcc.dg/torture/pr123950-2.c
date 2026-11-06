/* { dg-do run } */
/* { dg-require-effective-target double64 } */
/* { dg-require-effective-target fstack_protector } */
/* { dg-additional-options "-fstack-protector-all" } */

/* With the stack protector enabled, H arrives on the stack and is copied to a
   local stack slot on entry.  The copy must read all of the incoming slot,
   even though the function also reads the neighboring field of the local
   slot.  */

struct H { double a, b, c; };

__attribute__ ((noipa)) void
check (long long b, struct H *p)
{
  double d;

  __builtin_memcpy (&d, &b, sizeof (d));
  if (d != 2.0 || p->a != 1.0 || p->c != 3.0)
    __builtin_abort ();
}

__attribute__ ((noipa)) void
f (double r0, double r1, double r2, double r3, double r4, double r5, double r6,
   double r7, struct H h)
{
  long long b;

  __builtin_memcpy (&b, &h.b, sizeof (b));
  check (b, &h);
}

int
main (void)
{
  struct H h = { 1.0, 2.0, 3.0 };

  f (0, 0, 0, 0, 0, 0, 0, 0, h);
  return 0;
}
