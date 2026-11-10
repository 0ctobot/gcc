/* A weak definition of a const object can be replaced at link time by a
   strong definition from another translation unit, so the C front end
   mustn't fold a read of the object to the weak initializer.  The strong
   definitions are in weak-const-fold-1a.c.  */
/* { dg-do run } */
/* { dg-require-weak "" } */
/* { dg-options "-O2" } */
/* { dg-additional-sources "weak-const-fold-1a.c" } */

union U { int i; struct { short lo, hi; } s; };
struct HU { int k; union U u; };

__attribute__ ((weak)) const int x = 5;
__attribute__ ((weak)) const union U wu = { .i = 0x11223344 };

__attribute__ ((noipa)) int
get_x (void)
{
  return x;
}

/* Since wu's value isn't known until link time, .u.s.hi only replaces
   the high half of the value read from the prevailing wu.  */

__attribute__ ((noipa)) void
get_hu (struct HU *o)
{
  struct HU a = { .u = wu, .u.s.hi = 0x55 };
  *o = a;
}

int
main (void)
{
  union U ref = { .i = 0x55667788 };
  struct HU a;

  if (get_x () != 50)
    __builtin_abort ();
  get_hu (&a);
  if (a.u.s.lo != ref.s.lo || a.u.s.hi != 0x55)
    __builtin_abort ();
  return 0;
}
