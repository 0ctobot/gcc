/* PR c/105430 */
/* { dg-do run } */

/* A whole initializer known at compile time keeps the parts that later
   designators don't override.  */

extern void abort (void);

#define CK(c) do { if (!(c)) abort (); } while (0)

struct R { int b; int c; };
struct S { int a; struct R r; int e; };
struct T { struct S s; int z; };

const struct S s = { 1, { 2, 3 }, 4 };
const struct T t1 = { .s = s, .s.r.b = 5 };
const struct T t2 = { .s = s, .s.a = 7 };
const struct T t3 = { .s = s, .s.r.c = 8, .z = 9 };
const struct T t4 = { .z = 9, .s = s, .s.e = 10 };
static struct T t5 = { .s = s, .s.r.b = 5, .s.r.c = 6 };
const struct T t6 = { .s = t1.s, .s.a = 11 };
const struct T t7 = { .s.r = s.r, .s.r.c = 12 };
const struct T t8 = { .s = (s), .s.r.b = 5 };
struct T t9 = { .s = (struct S) { 1, { 2, 3 }, 4 }, .s.r.b = 5 };

const struct S as[2] = { [0] = s, [0].a = 21, [1] = s, [1].r.c = 22 };
const struct S ar[2] = { [0 ... 1] = s, [0].a = 21 };

struct A { int x; int a[3]; };
struct B { struct A a; };
const struct A ca = { 1, { 2, 3, 4 } };
const struct B b1 = { .a = ca, .a.a[1] = 30 };

union U { struct R r; long l; };
struct C { union U u; int i; };
const union U cu = { .r = { 1, 2 } };
const struct C c1 = { .u = cu, .u.r.b = 40 };
const struct C c2 = { .u = { .r = { 1, 2 } }, .u.r.b = 40 };

/* Copying a whole struct doesn't copy its flexible array member, so neither
   the member that follows nor the object that follows gets overlapped.  */
struct F { int a; int f[]; };
struct WF { struct F f; };
struct WFZ { struct F f; int z; };
const struct F cf = { 1, { 2, 3 } };
const struct WF w1 = { .f = cf, .f.a = 9 };
int after1 = 77;
const struct WFZ w2 = { .f = cf, .f.a = 9, .z = 5 };
int after2 = 88;

int
main (void)
{
  CK (t1.s.a == 1 && t1.s.r.b == 5 && t1.s.r.c == 3 && t1.s.e == 4);
  CK (t2.s.a == 7 && t2.s.r.b == 2 && t2.s.r.c == 3 && t2.s.e == 4);
  CK (t3.s.a == 1 && t3.s.r.b == 2 && t3.s.r.c == 8 && t3.s.e == 4);
  CK (t3.z == 9);
  CK (t4.s.a == 1 && t4.s.r.b == 2 && t4.s.r.c == 3 && t4.s.e == 10);
  CK (t4.z == 9);
  CK (t5.s.a == 1 && t5.s.r.b == 5 && t5.s.r.c == 6 && t5.s.e == 4);
  CK (t6.s.a == 11 && t6.s.r.b == 5 && t6.s.r.c == 3 && t6.s.e == 4);
  CK (t7.s.a == 0 && t7.s.r.b == 2 && t7.s.r.c == 12 && t7.s.e == 0);
  CK (t8.s.a == 1 && t8.s.r.b == 5 && t8.s.r.c == 3 && t8.s.e == 4);
  CK (t9.s.a == 1 && t9.s.r.b == 5 && t9.s.r.c == 3 && t9.s.e == 4);
  CK (as[0].a == 21 && as[0].r.b == 2 && as[0].r.c == 3 && as[0].e == 4);
  CK (as[1].a == 1 && as[1].r.b == 2 && as[1].r.c == 22 && as[1].e == 4);
  CK (ar[0].a == 21 && ar[0].r.b == 2 && ar[0].r.c == 3 && ar[0].e == 4);
  CK (ar[1].a == 1 && ar[1].r.b == 2 && ar[1].r.c == 3 && ar[1].e == 4);
  CK (b1.a.x == 1 && b1.a.a[0] == 2 && b1.a.a[1] == 30 && b1.a.a[2] == 4);
  CK (c1.u.r.b == 40 && c1.u.r.c == 2);
  CK (c2.u.r.b == 40 && c2.u.r.c == 2);
  CK (w1.f.a == 9 && after1 == 77);
  CK (w2.f.a == 9 && w2.z == 5 && after2 == 88);
  return 0;
}
