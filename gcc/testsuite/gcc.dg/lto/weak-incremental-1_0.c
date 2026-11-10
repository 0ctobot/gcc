/* { dg-lto-do link } */
/* { dg-lto-options { { -O2 -flto -fdump-tree-optimized } } } */
/* { dg-extra-ld-options { -r -nostdlib -flinker-output=nolto-rel } } */

/* An incremental link keeps a weak definition weak in its output, so a
   strong definition in an object added to the final link can replace it.
   The LTO code mustn't fold the weak initializer or inline the weak
   function.  */

__attribute__ ((weak)) const int ws = 1;

__attribute__ ((weak)) int
wf (int v)
{
  return v + 1;
}

/* Read ws through a pointer that only IPA propagation can resolve, which
   keeps the test independent of whether the front end folds a direct read
   of ws.  */

static __attribute__ ((noinline)) int
load (const int *p)
{
  return *p;
}

int
get (void)
{
  return load (&ws);
}

int
call (int v)
{
  return wf (v);
}

/* { dg-final { scan-ltrans-tree-dump-not "return 1;" "optimized" } } */
/* { dg-final { scan-ltrans-tree-dump "wf \\(v" "optimized" } } */
