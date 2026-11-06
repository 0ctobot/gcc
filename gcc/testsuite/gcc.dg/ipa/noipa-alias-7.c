/* A static noipa link is analyzed only after the public alias of it that keeps
   it alive, whatever the declaration order.  Resolving b re-points c directly
   onto a, which mustn't lose the noipa on b for calls through c.  */
/* { dg-do run } */
/* { dg-require-alias "" } */
/* { dg-options "-O2" } */

int
a (int v)
{
  return v + 1;
}

static int b (int) __attribute__ ((noipa, alias ("a")));
extern int c (int) __attribute__ ((alias ("b")));

int
main (void)
{
  int x = c (5);

  if (__builtin_constant_p (x))
    __builtin_abort ();
  return x != 6;
}
