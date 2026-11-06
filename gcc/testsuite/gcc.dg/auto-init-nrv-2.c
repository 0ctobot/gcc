/* Same as auto-init-nrv-1.c for the pattern initialization.  */
/* { dg-do run } */
/* { dg-options "-O2 -ftrivial-auto-var-init=pattern" } */

struct big { unsigned long long a[20]; };

__attribute__ ((noipa)) void
dirty_stack (void)
{
  volatile char buf[1024];
  for (int i = 0; i < 1024; i++)
    buf[i] = 0x5a;
}

__attribute__ ((noipa)) struct big
nrv (int set)
{
  struct big b;
  if (set)
    b.a[0] = 1;
  return b;
}

__attribute__ ((noipa)) void
check (void)
{
  struct big b = nrv (1);
  if (b.a[0] != 1)
    __builtin_abort ();
  for (int i = 1; i < 20; i++)
    if (b.a[i] != 0xfefefefefefefefeULL)
      __builtin_abort ();
}

int
main (void)
{
  dirty_stack ();
  check ();
  return 0;
}
