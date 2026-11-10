/* { dg-do compile } */
/* { dg-options "-ftrivial-auto-var-init=zero -fdump-tree-gimple" } */

/* A plain store to the variable or a part of it in its own initializer doesn't
   read it, so it doesn't need the automatic initialization ahead of the
   initializer, which would add a store to a volatile variable.  A compound
   assignment, an increment or a decrement reads the variable, and so does a
   left-hand side that uses the variable to compute where to store, even when
   it stores to a part of the variable.  */

extern int arr[];

struct S { int a, b; };
struct N { struct S s; int c; };
struct T { int arr[2]; int k; };
struct BF { int f : 3; int g; };
union U { int i; float f; };
struct P { struct S *p; int n; };

int
store_volatile (void)
{
  volatile int w1 = (w1 = 1, 2);
  return w1;
}

int
store_plain (int y)
{
  int w2 = (w2 = y, 3);
  return w2;
}

int
compound (void)
{
  int r1 = (r1 += 1, 2);
  return r1;
}

int
incr (void)
{
  int r2 = (r2++, 2);
  return r2;
}

int
decr (void)
{
  int r3 = (--r3, 2);
  return r3;
}

int
store_rhs (void)
{
  int r4 = (r4 = r4 + 1, 2);
  return r4;
}

int
index_lhs (void)
{
  int r5 = (arr[r5] = 1, 2);
  return r5;
}

int *
deref_lhs (int *q)
{
  int *r6 = (*r6 = 0, q);
  return r6;
}

int
store_member (void)
{
  volatile struct S w3 = (w3.a = 1, (struct S) { 2, 3 });
  return w3.b;
}

int
store_nested (void)
{
  volatile struct N w4 = (w4.s.b = 1, (struct N) { { 2, 3 }, 4 });
  return w4.c;
}

int
store_element (void)
{
  volatile struct T w5 = (w5.arr[1] = 1, (struct T) { { 4, 5 }, 6 });
  return w5.k;
}

int
store_bitfield (void)
{
  volatile struct BF w6 = (w6.f = 1, (struct BF) { 2, 3 });
  return w6.g;
}

int
store_union (void)
{
  volatile union U w7 = (w7.i = 5, (union U) { 6 });
  return w7.i;
}

double
store_real (void)
{
  volatile _Complex double w8 = (__real__ w8 = 1.0, 2.0);
  return __real__ w8;
}

int
index_part (void)
{
  struct T r7 = (r7.arr[r7.k] = 1, (struct T) { { 4, 5 }, 6 });
  return r7.k;
}

int
deref_part (struct S *q)
{
  struct P r8 = (r8.p->a = 1, (struct P) { q, 2 });
  return r8.n;
}

/* { dg-final { scan-tree-dump-not "\"w1\"" "gimple" } } */
/* { dg-final { scan-tree-dump-not "\"w2\"" "gimple" } } */
/* { dg-final { scan-tree-dump-not "\"w3\"" "gimple" } } */
/* { dg-final { scan-tree-dump-not "\"w4\"" "gimple" } } */
/* { dg-final { scan-tree-dump-not "\"w5\"" "gimple" } } */
/* { dg-final { scan-tree-dump-not "\"w6\"" "gimple" } } */
/* { dg-final { scan-tree-dump-not "\"w7\"" "gimple" } } */
/* { dg-final { scan-tree-dump-not "\"w8\"" "gimple" } } */
/* { dg-final { scan-tree-dump-times "DEFERRED_INIT" 8 "gimple" } } */
