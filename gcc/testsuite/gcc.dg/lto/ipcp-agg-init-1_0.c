/* { dg-lto-do run } */
/* { dg-lto-options {{-O3 -flto -fdump-ipa-cp-details}} } */

/* At WPA, the initializer of a static const table isn't read in until
   something asks for it.  IPA-CP must still find the target of the indirect
   call in run_flat in each table's initializer and specialize run_flat for
   each of them, as it does without LTO.  */

struct flat { int id; int flags; int (*fn) (int); };

static int cb_a (int v) { return v * 3 + 1; }
static int cb_b (int v) { return v * 5 + 2; }
extern int sink (int);

static const struct flat F1 = { 1, 2, cb_a }, F2 = { 1, 2, cb_b };

static __attribute__ ((noinline)) int
run_flat (const struct flat *p, int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    s += sink (p->fn (i));
  return s;
}

int
use_flat (int n)
{
  return run_flat (&F1, n) + run_flat (&F2, n + 1);
}

/* { dg-final { scan-wpa-ipa-dump-times "Creating a specialized node of run_flat" 2 "cp" } } */
