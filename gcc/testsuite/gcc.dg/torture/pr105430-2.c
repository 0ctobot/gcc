/* PR c/105430 */
/* PR c/63944 */
/* { dg-do run } */

/* A whole initializer only known at run time keeps the parts that later
   designators don't override, and gets evaluated only once.  */

extern void abort (void);

#define CK(c) do { if (!(c)) abort (); } while (0)

struct R { int b; int c; };
struct S { int a; struct R r; int e; };
struct T { struct S s; int z; };
struct A { int x; int a[3]; struct R ra[2]; char str[4]; };
struct B { struct A a; int y; };
union U { struct R r; long l; int i; };
struct C { union U u; int i; };
struct BF { int k : 3; int : 2; int m : 5; unsigned n : 7; };
struct DBF { struct BF f; };
struct AN { int p; struct { int q; int w; }; union { int v; float g; }; };
struct DAN { struct AN n; };
struct VS { volatile int a; int b; };
struct DVS { struct VS s; };
struct AF { _Atomic int a; int b; const int d; };
struct DAF { struct AF c; };

static int calls;

__attribute__((noipa)) struct S
f (void)
{
  struct S x = { 1, { 2, 3 }, 4 };
  calls++;
  return x;
}

__attribute__((noipa)) union U
fu (void)
{
  union U x = { .r = { 1, 2 } };
  calls++;
  return x;
}

__attribute__((noipa)) void
lvalues (struct S *p, int i)
{
  struct S x = *p;
  struct T t1 = { .s = x, .s.r.b = 5 };
  CK (t1.s.a == 1 && t1.s.r.b == 5 && t1.s.r.c == 3 && t1.s.e == 4);
  CK (t1.z == 0);
  struct T t2 = { .s = *p, .s.a = 7 };
  CK (t2.s.a == 7 && t2.s.r.b == 2 && t2.s.r.c == 3 && t2.s.e == 4);
  struct S arr[2] = { *p, *p };
  struct T t3 = { .s = arr[i], .s.r.c = 8, .z = 9 };
  CK (t3.s.a == 1 && t3.s.r.b == 2 && t3.s.r.c == 8 && t3.s.e == 4);
  CK (t3.z == 9);
  struct T t4 = { .z = 9, .s = x, .s.e = 10 };
  CK (t4.s.a == 1 && t4.s.r.b == 2 && t4.s.r.c == 3 && t4.s.e == 10);
  CK (t4.z == 9);
  struct T t5 = { .s.r = x.r, .s.r.c = 12 };
  CK (t5.s.a == 0 && t5.s.r.b == 2 && t5.s.r.c == 12 && t5.s.e == 0);
  struct T t6 = { .s = i ? x : *p, .s.e = 13 };
  CK (t6.s.a == 1 && t6.s.r.b == 2 && t6.s.r.c == 3 && t6.s.e == 13);
  struct S a1[2] = { [0] = x, [0].a = 21, [1] = x, [1].r.c = 22 };
  CK (a1[0].a == 21 && a1[0].r.b == 2 && a1[0].r.c == 3 && a1[0].e == 4);
  CK (a1[1].a == 1 && a1[1].r.b == 2 && a1[1].r.c == 22 && a1[1].e == 4);
  struct S a2[2] = { [0 ... 1] = x, [0].a = 21 };
  CK (a2[0].a == 21 && a2[0].r.b == 2 && a2[0].r.c == 3 && a2[0].e == 4);
  CK (a2[1].a == 1 && a2[1].r.b == 2 && a2[1].r.c == 3 && a2[1].e == 4);
  /* Replace a part, then enter it again.  */
  struct T t7 = { .s = x, .s.r = p->r, .s.r.b = 5 };
  CK (t7.s.a == 1 && t7.s.r.b == 5 && t7.s.r.c == 3 && t7.s.e == 4);
  /* Enter the member, replace all of it, then enter it again.  */
  struct T t8 = { .s = x, .s.r.b = 5, .s = *p, .s.a = 9 };
  CK (t8.s.a == 9 && t8.s.r.b == 2 && t8.s.r.c == 3 && t8.s.e == 4);
  /* Two separate entries after the same whole value.  */
  struct T t9 = { .s = x, .s.r.b = 5, .z = 3, .s.r.c = 6 };
  CK (t9.s.a == 1 && t9.s.r.b == 5 && t9.s.r.c == 6 && t9.s.e == 4);
  CK (t9.z == 3);
  /* Brace elision into the member.  */
  struct T t10 = { .s = *p, .s = 9 };
  CK (t10.s.a == 9 && t10.s.r.b == 2 && t10.s.r.c == 3 && t10.s.e == 4);
  /* A const local object.  */
  const struct S cs = { 1, { 2, 3 }, 4 };
  struct T t11 = { .s = cs, .s.r.b = 5 };
  CK (t11.s.a == 1 && t11.s.r.b == 5 && t11.s.r.c == 3 && t11.s.e == 4);
  static const struct S scs = { 1, { 2, 3 }, 4 };
  struct T t12 = { .s = scs, .s.r.b = 5 };
  CK (t12.s.a == 1 && t12.s.r.b == 5 && t12.s.r.c == 3 && t12.s.e == 4);
}

__attribute__((noipa)) void
once (struct S *p)
{
  calls = 0;
  struct T t1 = { .s = f (), .s.r.b = 5 };
  CK (calls == 1);
  CK (t1.s.a == 1 && t1.s.r.b == 5 && t1.s.r.c == 3 && t1.s.e == 4);
  calls = 0;
  struct T t2 = { .s = f (), .s.r.b = 5, .s.a = 6 };
  CK (calls == 1);
  CK (t2.s.a == 6 && t2.s.r.b == 5 && t2.s.r.c == 3 && t2.s.e == 4);
  calls = 0;
  struct C c = { .u = fu (), .u.r.b = 40 };
  CK (calls == 1);
  CK (c.u.r.b == 40 && c.u.r.c == 2);
  calls = 0;
  struct T t3 = { .s = (calls++, *p), .s.a = 7 };
  CK (calls == 1);
  CK (t3.s.a == 7 && t3.s.r.b == 2 && t3.s.r.c == 3 && t3.s.e == 4);
  calls = 0;
  struct T t4 = { .s = ({ calls++; *p; }), .s.a = 7 };
  CK (calls == 1);
  CK (t4.s.a == 7 && t4.s.r.b == 2 && t4.s.r.c == 3 && t4.s.e == 4);
}

__attribute__((noipa)) void
literal (int v)
{
  struct T t = { .s = (struct S) { v, { 2, 3 }, 4 }, .s.r.b = 5 };
  CK (t.s.a == 1 && t.s.r.b == 5 && t.s.r.c == 3 && t.s.e == 4);
}

__attribute__((noipa)) void
arrays (struct A *p)
{
  struct B b1 = { .a = *p, .a.a[1] = 30 };
  CK (b1.a.x == 1 && b1.a.a[0] == 2 && b1.a.a[1] == 30 && b1.a.a[2] == 4);
  CK (b1.a.ra[0].b == 5 && b1.a.ra[1].c == 8 && b1.a.str[1] == 'y');
  struct B b2 = { .a = *p, .a.ra[1].b = 31 };
  CK (b2.a.x == 1 && b2.a.a[0] == 2 && b2.a.a[1] == 3 && b2.a.a[2] == 4);
  CK (b2.a.ra[0].b == 5 && b2.a.ra[0].c == 6);
  CK (b2.a.ra[1].b == 31 && b2.a.ra[1].c == 8);
  struct B b3 = { .a = *p, .a.x = 32 };
  CK (b3.a.x == 32 && b3.a.a[0] == 2 && b3.a.a[1] == 3 && b3.a.a[2] == 4);
  CK (b3.a.ra[0].b == 5 && b3.a.ra[1].c == 8 && b3.a.str[0] == 'x');
  struct B b4 = { .a = *p, .a.str[1] = 'z' };
  CK (b4.a.str[0] == 'x' && b4.a.str[1] == 'z' && b4.a.str[2] == 'w');
  struct B b5 = { .a = *p, .a.str = "abc" };
  CK (b5.a.x == 1 && b5.a.str[0] == 'a' && b5.a.a[1] == 3);
}

__attribute__((noipa)) void
unions (union U u)
{
  struct C c1 = { .u = u, .u.r.b = 40, .i = 2 };
  CK (c1.u.r.b == 40 && c1.u.r.c == 2 && c1.i == 2);
  struct C c2 = { .u = u, .u.r.c = 41 };
  CK (c2.u.r.b == 1 && c2.u.r.c == 41);
  struct C c3 = { .u = u, .u.l = 42 };
  CK (c3.u.l == 42);
  /* Brace elision into the first member of the union.  */
  struct C c4 = { .u = u, .u = 5 };
  CK (c4.u.r.b == 5 && c4.u.r.c == 2);
}

__attribute__((noipa)) void
bitfields (struct BF *p, struct AN *q)
{
  struct DBF d = { .f = *p, .f.m = 3 };
  CK (d.f.k == -2 && d.f.m == 3 && d.f.n == 100);
  struct DAN n1 = { .n = *q, .n.q = 7 };
  CK (n1.n.p == 1 && n1.n.q == 7 && n1.n.w == 3 && n1.n.v == 4);
  struct DAN n2 = { .n = *q, .n.v = 8 };
  CK (n2.n.p == 1 && n2.n.q == 2 && n2.n.w == 3 && n2.n.v == 8);
}

__attribute__((noipa)) void
qualified (volatile struct S *p, struct VS *q, struct AF *r)
{
  struct T t = { .s = *p, .s.a = 7 };
  CK (t.s.a == 7 && t.s.r.b == 2 && t.s.r.c == 3 && t.s.e == 4);
  struct DVS d = { .s = *q, .s.b = 7 };
  CK (d.s.a == 1 && d.s.b == 7);
  struct DAF c = { .c = *r, .c.b = 9 };
  CK (c.c.a == 1 && c.c.b == 9 && c.c.d == 3);
}

/* The whole value can leave the function before any part gets read.  */

__attribute__((noipa)) int
early_return (struct S *p, int c)
{
  struct T t = { .s = ({ if (c) return 1; *p; }), .s.a = 3 };
  return t.s.r.b;
}

int
main (void)
{
  struct S x = { 1, { 2, 3 }, 4 };
  lvalues (&x, 1);
  once (&x);
  literal (1);
  struct A a = { 1, { 2, 3, 4 }, { { 5, 6 }, { 7, 8 } }, "xyw" };
  arrays (&a);
  union U u = { .r = { 1, 2 } };
  unions (u);
  struct BF bf = { -2, 9, 100 };
  struct AN an = { 1, { 2, 3 }, { 4 } };
  bitfields (&bf, &an);
  struct VS vs = { 1, 2 };
  struct AF af = { 1, 2, 3 };
  qualified (&x, &vs, &af);
  CK (early_return (&x, 0) == 2);
  CK (early_return (&x, 1) == 1);
  return 0;
}
