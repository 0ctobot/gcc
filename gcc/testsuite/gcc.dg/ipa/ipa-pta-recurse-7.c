/* { dg-do run } */
/* { dg-require-effective-target trampolines } */
/* { dg-options "-O2 -fipa-pta" } */

/* The leaf activation of walk stores to the count of its parent activation
   through the address that the parent's nested function returns.  The
   parent's frame is only reachable from walk's parameter through the static
   chain of that nested function, so IPA PTA has to follow the static chain
   to see that count can belong to another activation.  Otherwise the store
   is deleted as dead at function exit.  */

typedef int *(*getter) (void);

static int __attribute__ ((noinline))
walk (unsigned n, getter parent)
{
  int count = 0;
  __attribute__ ((noinline)) int *address (void) { return &count; }

  if (n)
    {
      walk (n - 1, address);
      return count;
    }
  if (parent)
    *parent () = 7;
  return 0;
}

volatile unsigned depth = 1;

int
main (void)
{
  if (walk (depth, 0) != 7)
    __builtin_abort ();
  return 0;
}
