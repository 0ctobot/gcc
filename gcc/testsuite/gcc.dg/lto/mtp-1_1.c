/* { dg-options "-mtp=tpidrro_el0" } */

__thread int v, w;

void *
ro_tp (void)
{
  return __builtin_thread_pointer ();
}

/* The addresses of the thread-local variables only appear as PHI
   arguments here.  */
int *
pick (int c)
{
  return c ? &v : &w;
}

__attribute__ ((noipa)) int *
pick_ref (int c)
{
  return c ? &v : &w;
}

int
scale (int x)
{
  return x * 3 + 1;
}
