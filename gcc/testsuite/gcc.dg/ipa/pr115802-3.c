/* PR ipa/115802 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

/* Like pr115802.c, but the functions that other threads synchronize with
   call no barrier builtin.  One of them has an asm statement that clobbers
   memory, and the others use an atomic compare and exchange with a local
   expected value, which is folded into an internal function call before
   ipa-reference runs.  Neither of these has a call graph edge, but both are
   threading barriers that order our accesses to the statics against those
   of other threads.  */

static int gate;
static int payload_asm;
static int payload_cas;

__attribute__ ((noinline)) static void
barrier (void)
{
  __asm__ __volatile__ ("" : : : "memory");
}

__attribute__ ((noinline)) static void
lock (void)
{
  int expected;

  do
    expected = 0;
  while (!__atomic_compare_exchange_n (&gate, &expected, 1, 0,
				       __ATOMIC_ACQUIRE, __ATOMIC_RELAXED));
}

__attribute__ ((noinline)) static void
unlock (void)
{
  int expected = 1;

  __atomic_compare_exchange_n (&gate, &expected, 0, 0, __ATOMIC_RELEASE,
			       __ATOMIC_RELAXED);
}

/* Another thread can store to the statics at the barrier, so they have to
   be loaded before the loop and again on every iteration.  */

void
wait_for_payload_asm (void)
{
  while (payload_asm != 7)
    barrier ();
}

void
wait_for_payload_cas (void)
{
  lock ();
  while (payload_cas != 7)
    {
      unlock ();
      lock ();
    }
  unlock ();
}

/* The second load cannot reuse the value of the first one.  */

int
read_payload_twice_asm (void)
{
  int first, second;

  first = payload_asm;
  barrier ();
  second = payload_asm;
  return first - second;
}

int
read_payload_twice_cas (void)
{
  int first, second;

  lock ();
  first = payload_cas;
  unlock ();
  lock ();
  second = payload_cas;
  unlock ();
  return first - second;
}

/* Another thread can read the first value at the barrier, so the first store
   isn't dead.  */

void
set_payload_twice_asm (void)
{
  payload_asm = 1;
  barrier ();
  payload_asm = 7;
}

void
set_payload_twice_cas (void)
{
  lock ();
  payload_cas = 1;
  unlock ();
  lock ();
  payload_cas = 7;
  unlock ();
}

/* { dg-final { scan-tree-dump-times "= payload_asm;" 4 "optimized" } } */
/* { dg-final { scan-tree-dump-times "= payload_cas;" 4 "optimized" } } */
/* { dg-final { scan-tree-dump "payload_asm = 1;" "optimized" } } */
/* { dg-final { scan-tree-dump "payload_cas = 1;" "optimized" } } */
