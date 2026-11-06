/* A load that may throw can be the only read of a local variable, so RTL DSE
   has to keep the stores to it.  */
/* { dg-do run } */
/* { dg-options "-O2 -fnon-call-exceptions" } */

__attribute__ ((noipa)) void
check (int x)
{
  if (x != 2)
    __builtin_abort ();
}

int
main (void)
{
  int a[4];
  a[0] = 2;
  a[1] = 2;
  a[2] = 2;
  a[3] = 2;
  for (unsigned int i = 0; i < 4; i++)
    check (a[i]);
  return 0;
}
