/* PR ipa/115802 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

/* PAYLOAD is only accessed with the spinlock held and its address is never
   taken, so ipa-reference tracks which functions read and write it.  The
   lock and unlock functions never name PAYLOAD, but the atomic builtins
   they call are threading barriers, which order our accesses to PAYLOAD
   against those of other threads.  As such, calls to lock and unlock have
   to count as reading and writing PAYLOAD.  */

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

/* Another thread can store to PAYLOAD while the lock is dropped, so it has
   to be loaded before the loop and again on every iteration.  */

void
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

/* The second load cannot reuse the value of the first one.  */

int
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

/* Another thread can read the first value while the lock is dropped, so the
   first store isn't dead.  */

void
set_payload_twice (void)
{
  lock ();
  payload = 1;
  unlock ();
  lock ();
  payload = 7;
  unlock ();
}

/* { dg-final { scan-tree-dump-times "= payload;" 4 "optimized" } } */
/* { dg-final { scan-tree-dump "payload = 1;" "optimized" } } */
