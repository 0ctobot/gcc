/* A copy of a struct doesn't include its flexible array member, even when the
   initializer of a const object stands in for the copy, and nothing reserves
   room for that member's data in the object holding the copy.  */
/* { dg-do run } */
/* { dg-options "-std=gnu11 -O2" } */

struct F { int a; int f[]; };
struct WF { struct F f; int z; };
struct G { char c; struct F f; int z; };
union U { struct F f; int x[3]; };

static const struct F cf = { 1, { 2, 3 } };
static const struct WF cw = { .f = cf, .z = 7 };

struct F t0 = cf;
int after0 = 10;
struct WF t1 = { .f = cf };
struct WF t2 = { .f = cf, .z = 3 };
struct WF t3[2] = { { .f = cf, .z = 4 }, { .f = cf, .z = 5 } };
struct G t4 = { 'x', cf, 6 };
union U t5 = { .f = cf };
const struct WF t6 = cw;
struct WF t7 = { .f = cf, .f.a = 9, .z = 8 };

int
main (void)
{
  static struct F s = cf;

  if (t0.a != 1 || after0 != 10 || sizeof (t0) != sizeof (int))
    __builtin_abort ();
  if (t1.f.a != 1 || t1.z != 0)
    __builtin_abort ();
  if (t2.f.a != 1 || t2.z != 3)
    __builtin_abort ();
  if (t3[0].f.a != 1 || t3[0].z != 4 || t3[1].f.a != 1 || t3[1].z != 5)
    __builtin_abort ();
  if (t4.c != 'x' || t4.f.a != 1 || t4.z != 6)
    __builtin_abort ();
  if (t5.f.a != 1 || t5.x[1] != 0 || t5.x[2] != 0)
    __builtin_abort ();
  if (t6.f.a != 1 || t6.z != 7)
    __builtin_abort ();
  if (t7.f.a != 9 || t7.z != 8)
    __builtin_abort ();
  if (s.a != 1)
    __builtin_abort ();
  return 0;
}
