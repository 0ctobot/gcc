/* PR ipa/115802 */
/* { dg-do link } */
/* { dg-require-effective-target lto } */
/* { dg-require-effective-target linker_plugin } */
/* { dg-options "-O2 -flto -fdump-tree-optimized" } */

/* Like pr115802.c, but with ipa-reference running at link time, where it
   only sees the calls to the atomic builtins as call graph edges.  */

static int gate;
static int payload;

__attribute__ ((noinline)) static void
lock (void)
{
  while (__atomic_exchange_n (&gate, 1, __ATOMIC_ACQUIRE))
    ;
}

__attribute__ ((noinline)) static void
unlock (void)
{
  __atomic_store_n (&gate, 0, __ATOMIC_RELEASE);
}

__attribute__ ((noinline)) void
wait_for_payload (void)
{
  lock ();
  while (payload != 7)
    {
      unlock ();
      lock ();
    }
  unlock ();
}

__attribute__ ((noinline)) int
read_payload_twice (void)
{
  int first, second;

  lock ();
  first = payload;
  unlock ();
  lock ();
  second = payload;
  unlock ();
  return first - second;
}

__attribute__ ((noinline)) void
set_payload_twice (void)
{
  lock ();
  payload = 1;
  unlock ();
  lock ();
  payload = 7;
  unlock ();
}

int
main (void)
{
  set_payload_twice ();
  wait_for_payload ();
  return read_payload_twice ();
}

/* { dg-final { scan-ltrans-tree-dump-times "= payload" 4 "optimized" } } */
/* { dg-final { scan-ltrans-tree-dump "payload\[^ \]* = 1;" "optimized" } } */
