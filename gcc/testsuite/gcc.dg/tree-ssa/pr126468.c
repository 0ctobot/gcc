/* PR middle-end/126468 */
/* { dg-do compile } */
/* { dg-require-effective-target int32 } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O2 -fwrapv -fno-tree-ccp -std=gnu11 -w -fdump-tree-optimized" } */

/* A constant with TREE_OVERFLOW set, which dumps print with (OVF), isn't
   supposed to appear in the IL.  Each function below used to put one there
   through a different path.  */

long g1, g2, g3;
int g4, g5;

/* Final value replacement in sccp.  */

void
scev (void)
{
  long x = 0;
  do
    x += 60976676962555186L;
  while (x > 0);
  g1 = x;
}

/* Constant merging in reassoc.  */

long
reassoc (long a, long b, long c)
{
  long t = (a * 3) + 0x7000000000000000L;
  long u = (b * 5) + t;
  long v = (c * 7) + u;
  return v + 0x7000000000000000L;
}

/* Pass-through jump function arithmetic in IPA-CP.  */

__attribute__((noinline)) static long
ipa_leaf (long x, long k)
{
  long r = 0;
  for (long i = 0; i < k; i++)
    r += x + i;
  g2 = x;
  return r;
}

__attribute__((noinline)) static long
ipa_mid (long y, long k)
{
  return (ipa_leaf (y + 0x7000000000000000L, k + 3)
	  + ipa_leaf (y + 0x7000000000000000L, k * 3));
}

long
ipa (long k)
{
  return (ipa_mid (0x7000000000000000L, k)
	  + ipa_mid (0x7000000000000000L, k * 7));
}

/* Narrowing conversion of a parameter value in IPA-CP.  */

static int narrow_leaf ();

__attribute__((noinline)) static int
narrow_mid (long y)
{
  return narrow_leaf (y);
}

__attribute__((noinline)) static int
narrow_leaf (x)
     int x;
{
  g4 = x;
  return 0;
}

int
narrow (void)
{
  return narrow_mid (0x1ffffffffL);
}

/* Narrowing conversion of an argument in the inliner.  */

static inline int inl_leaf ();

int
inl (void)
{
  return inl_leaf (0x1ffffffffL);
}

static inline int
inl_leaf (x)
     int x;
{
  g5 = x;
  return 0;
}

/* Reading a static initializer.  */

static const struct { long a; long b; } s = { __LONG_MAX__ + 1L, 2 };

void
ctor (void)
{
  g3 = s.a;
}

/* Case label from the front end.  */

int
label (int i)
{
  switch (i)
    {
    case (2147483647 * 4) % 4 - 2:
      return 5;
    }
  return 7;
}

/* { dg-final { scan-tree-dump-not "\\(OVF\\)" "optimized" } } */
