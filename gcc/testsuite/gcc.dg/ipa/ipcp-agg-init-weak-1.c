/* { dg-do compile } */
/* { dg-require-weak "" } */
/* { dg-options "-O3 -fdump-ipa-cp-details" } */

/* IPA-CP must not take the target of an indirect call from the initializer
   of a table that can be replaced at link time.  Each table gets its own
   clone of the function it's passed to if IPA-CP finds the target in the
   initializer.  */

struct ops { int id; int (*fn) (int); };

static int cb_a (int v) { return v * 3 + 1; }
static int cb_b (int v) { return v * 5 + 2; }
extern int sink (int);

/* A strong definition in another object can replace these.  */
__attribute__ ((weak)) const struct ops W1 = { 1, cb_a };
__attribute__ ((weak)) const struct ops W2 = { 1, cb_b };

static __attribute__ ((noinline)) int
run_weak (const struct ops *p, int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    s += sink (p->fn (i) + 55);
  return s;
}

int
use_weak (int n)
{
  return run_weak (&W1, n) + run_weak (&W2, n + 1);
}

/* { dg-final { scan-ipa-dump-not "Creating a specialized node of run_weak" "cp" } } */
