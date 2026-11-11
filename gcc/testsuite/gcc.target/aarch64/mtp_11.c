/* { dg-do compile } */
/* { dg-options "-O2 -mtp=sp_el0" } */

/* __builtin_thread_pointer reads sp_el0, and every function below reads
   it exactly once, however many times the source asks for it.  */

extern void g (void);

void *
tp (void)
{
  return __builtin_thread_pointer ();
}

long
twice (void)
{
  long *a = (long *) __builtin_thread_pointer ();
  long *b = (long *) __builtin_thread_pointer ();
  return a[1] + b[2];
}

long
branch (int x)
{
  long *a = (long *) __builtin_thread_pointer ();
  long r = a[1];
  if (x)
    {
      long *b = (long *) __builtin_thread_pointer ();
      r += b[3];
    }
  return r;
}

long
across_call (void)
{
  long *a = (long *) __builtin_thread_pointer ();
  long r = a[1];
  g ();
  long *b = (long *) __builtin_thread_pointer ();
  return r + b[2];
}

/* { dg-final { scan-assembler-times {mrs\tx[0-9]+, sp_el0} 4 } } */
/* { dg-final { scan-assembler-not {tpidr} } } */
