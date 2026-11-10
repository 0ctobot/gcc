/* gops stores fa to both fields, and a const automatic array of records
   also holds fb for field f.  Calls through f must not be speculated to
   fa, while calls through g still are.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-ipa-devirt" } */

typedef int (*fn_t) (int);

struct ops { long pad; fn_t f; fn_t g; };

int __attribute__ ((noinline)) fa (int x) { return x + 1; }
int __attribute__ ((noinline)) fb (int x) { return x + 100; }

struct ops gops = { 0, fa, fa };

static void __attribute__ ((noinline, noclone))
pick (struct ops *o, int i)
{
  const struct ops t[4] = { { 0, fb, fa }, { 1, fa, fa },
			    { 2, fb, fa }, { 3, fa, fa } };
  *o = t[i];
}

void
do_store (struct ops *o, int i)
{
  pick (o, i);
}

int __attribute__ ((noinline, noclone))
call_f (struct ops *o, int x)
{
  return o->f (x);
}

int __attribute__ ((noinline, noclone))
call_g (struct ops *o, int x)
{
  return o->g (x);
}

/* { dg-final { scan-ipa-dump-not "speculative call call_f/" "devirt" } } */
/* { dg-final { scan-ipa-dump "speculative call call_g/\[0-9\]+ => fa/" "devirt" } } */
