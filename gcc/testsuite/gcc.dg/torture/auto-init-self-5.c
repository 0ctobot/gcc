/* { dg-do run } */
/* { dg-additional-options "-ftrivial-auto-var-init=zero -Wno-uninitialized" } */

/* A goto or a case label that skips the declaration of a variable lets code
   below the declaration store to the variable before a jump back runs its
   initializer.  An initializer that reads the variable has to see that stored
   value, so the variable doesn't get the automatic initialization ahead of
   it.  */

__attribute__ ((noipa)) int
by_name (void)
{
  goto after;
 before:;
  int x = x + 1;
  return x;
 after:
  x = 41;
  goto before;
}

__attribute__ ((noipa)) int
into_block (void)
{
  goto in;
  {
  top:;
    int x = x + 1;
    return x;
  in:
    x = 41;
    goto top;
  }
}

__attribute__ ((noipa)) int
at_case (int k)
{
  switch (k)
    {
    case 0:
    again:;
      int x = x + 1;
      return x;
    case 1:
      x = 41;
      goto again;
    }
  return -1;
}

int
main (void)
{
  if (by_name () != 42)
    __builtin_abort ();
  if (into_block () != 42)
    __builtin_abort ();
  if (at_case (1) != 42)
    __builtin_abort ();
  return 0;
}
