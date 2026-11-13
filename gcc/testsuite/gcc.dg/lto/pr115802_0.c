/* PR ipa/115802 */
/* { dg-lto-do link } */
/* { dg-require-effective-target linker_plugin } */
/* { dg-lto-options { { -O2 -flto -fdump-tree-optimized } } } */

/* Like gcc.dg/ipa/pr115802-3.c, but with the barriers in another unit and
   ipa-reference running at link time, where it doesn't have the function
   bodies and only knows about the barriers from what was streamed.  */

extern void barrier (void);
extern void lock (void);
extern void unlock (void);

static int payload_asm;
static int payload_cas;

__attribute__ ((noinline)) void
wait_for_payload_asm (void)
{
  while (payload_asm != 7)
    barrier ();
}

__attribute__ ((noinline)) void
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

__attribute__ ((noinline)) int
read_payload_twice_asm (void)
{
  int first, second;

  first = payload_asm;
  barrier ();
  second = payload_asm;
  return first - second;
}

__attribute__ ((noinline)) int
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

__attribute__ ((noinline)) void
set_payload_twice_asm (void)
{
  payload_asm = 1;
  barrier ();
  payload_asm = 7;
}

__attribute__ ((noinline)) void
set_payload_twice_cas (void)
{
  lock ();
  payload_cas = 1;
  unlock ();
  lock ();
  payload_cas = 7;
  unlock ();
}

int
main (void)
{
  set_payload_twice_asm ();
  set_payload_twice_cas ();
  wait_for_payload_asm ();
  wait_for_payload_cas ();
  return read_payload_twice_asm () + read_payload_twice_cas ();
}

/* { dg-final { scan-ltrans-tree-dump-times "= payload_asm" 4 "optimized" } } */
/* { dg-final { scan-ltrans-tree-dump-times "= payload_cas" 4 "optimized" } } */
/* { dg-final { scan-ltrans-tree-dump "payload_asm\[^ \]* = 1;" "optimized" } } */
/* { dg-final { scan-ltrans-tree-dump "payload_cas\[^ \]* = 1;" "optimized" } } */
