/* PR tree-optimization/128600 */
/* { dg-do run } */
/* { dg-require-effective-target int32plus } */

/* The product takes the values -1073709056 and 2147450880, which fit in int,
   but its step 3221159936 doesn't.  The wrapped step made niter bound the
   multiplication to a single execution, so only the first value was
   stored.  */

int g;

__attribute__((noipa)) void
f (int n)
{
  for (int i = 0; i < n; i++)
    g = (i * 65536 - 32768) * (i * 32768 + 32767);
}

int
main (void)
{
  f (2);
  if (g != 2147450880)
    __builtin_abort ();
  return 0;
}
