/* { dg-do compile } */
/* { dg-options "-O3 -fdump-ipa-cp-details" } */

/* IPA-CP must find the target of an indirect call in a static initializer
   when the field it's loaded from comes after a nested aggregate, and when
   it's in an element of an array other than the first.  Each table gets its
   own clone of the function it's passed to.  */

struct inner { int id; int flags; };
struct nest { struct inner hdr; int (*fn) (int); };
struct desc { const char *name; int (*fn) (int); };
struct tbl { int n; struct desc d[3]; };

static int cb_a (int v) { return v * 3 + 1; }
static int cb_b (int v) { return v * 5 + 2; }
static int cb_c (int v) { return v ^ 0x55; }
extern int sink (int);

static const struct nest N1 = { { 1, 2 }, cb_a }, N2 = { { 1, 2 }, cb_b };

static const struct tbl T1 = { 3, { { "a", cb_a }, { "b", cb_b }, { "c", cb_c } } };
static const struct tbl T2 = { 3, { { "x", cb_c }, { "y", cb_a }, { "z", cb_b } } };

static __attribute__ ((noinline)) int
run_nest (const struct nest *p, int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    s += sink (p->fn (i));
  return s;
}

static __attribute__ ((noinline)) int
run_tbl (const struct tbl *p, int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    s += sink (p->d[1].fn (i));
  return s;
}

int
use_nest (int n)
{
  return run_nest (&N1, n) + run_nest (&N2, n + 1);
}

int
use_tbl (int n)
{
  return run_tbl (&T1, n) + run_tbl (&T2, n + 1);
}

/* { dg-final { scan-ipa-dump-times "Creating a specialized node of run_nest" 2 "cp" } } */
/* { dg-final { scan-ipa-dump-times "Creating a specialized node of run_tbl" 2 "cp" } } */
