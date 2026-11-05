/* PR ipa/128074 */
/* { dg-do run } */
/* { dg-additional-options "-foptimize-sibling-calls" } */

/* Tail recursion elimination stores the next iteration's argument into an
   aggregate parameter, so a const parameter doesn't keep its incoming value.
   Nothing may assume that it does.  */

struct S { int x; };
struct L { int x; int pad[15]; };
union U { int x; float f; };
struct A { const int v[4]; };

const struct S cs = { 1 };

/* IPA-CP must not replace a.x with its known incoming value.  */

__attribute__((noinline)) int
ipcp (const struct S a, int n)
{
  if (n)
    {
      struct S t = { a.x + 1 };
      return ipcp (t, n - 1);
    }
  return a.x;
}

/* Likewise for an aggregate some targets pass by invisible reference.  */

__attribute__((noinline)) int
ipcp_large (const struct L a, int n)
{
  if (n)
    {
      struct L t = { a.x + 1, { 5 } };
      return ipcp_large (t, n - 1);
    }
  return a.x + a.pad[0];
}

__attribute__((noinline)) int
ipcp_union (const union U a, int n)
{
  if (n)
    {
      union U t;
      t.x = a.x + 1;
      return ipcp_union (t, n - 1);
    }
  return a.x;
}

__attribute__((noinline)) int
ipcp_array (const struct A a, int n)
{
  if (n)
    {
      struct A t = { { a.v[0] + 1, a.v[1] + 2, a.v[2] + 3, a.v[3] + 4 } };
      return ipcp_array (t, n - 1);
    }
  return a.v[0] + a.v[1] + a.v[2] + a.v[3];
}

/* Swapping parameters stores to them from a temporary.  */

static __attribute__((noinline)) int
ipcp_swap (const struct S a, const struct S b, int n)
{
  if (n)
    return ipcp_swap (b, a, n - 1);
  return a.x * 10 + b.x;
}

/* The inliner must not map a straight to the caller's object, or else the
   store clobbers the caller's copy or faults on read-only memory.  */

static inline int
inl (const struct S a, int n)
{
  if (n)
    {
      struct S t = { a.x + 1 };
      return inl (t, n - 1);
    }
  return a.x;
}

__attribute__((noipa)) int
inl_local (int n, int k)
{
  struct S v;
  v.x = k;
  int r = inl (v, n);
  return r * 100 + v.x;
}

__attribute__((noipa)) int
inl_parm (const struct S v, int n)
{
  int r = inl (v, n);
  return r * 100 + v.x;
}

__attribute__((noipa)) int
inl_global (int n)
{
  return inl (cs, n);
}

int
main (void)
{
  volatile int n = 7, k = 1;
  struct S s0 = { 0 }, s1 = { 1 }, s2 = { 2 };
  struct L l = { 3, { 1 } };
  union U u;
  struct A a = { { 0, 0, 0, 0 } };

  u.x = 0;
  if (ipcp (s0, n) != 7)
    __builtin_abort ();
  if (ipcp_large (l, n) != 15)
    __builtin_abort ();
  if (ipcp_union (u, n) != 7)
    __builtin_abort ();
  if (ipcp_array (a, n) != 70)
    __builtin_abort ();
  if (ipcp_swap (s1, s2, n) != 21)
    __builtin_abort ();
  if (inl_local (n, k) != 801)
    __builtin_abort ();
  if (inl_parm (s1, n) != 801)
    __builtin_abort ();
  if (inl_global (n) != 8)
    __builtin_abort ();
  return 0;
}
