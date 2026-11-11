/* A weak definition of a const object can be replaced at link time by a
   strong definition from another translation unit, so the gimplifier
   mustn't copy the object from the weak initializer or call the function
   a weak const function pointer is initialized with.  The strong
   definitions are in weak-const-copy-1a.c.  */
/* { dg-do run } */
/* { dg-require-weak "" } */
/* { dg-additional-sources "weak-const-copy-1a.c" } */

struct P { int x, y; };

extern __SIZE_TYPE__ strlen (const char *);

__attribute__ ((weak)) const struct P wp = { 1, 2 };
__attribute__ ((weak)) __SIZE_TYPE__ (*const wl) (const char *) = strlen;

__attribute__ ((noipa)) struct P
get_p (void)
{
  return wp;
}

__attribute__ ((noipa)) int
sum_p (void)
{
  struct P l = wp;
  return l.x + l.y;
}

__attribute__ ((noipa)) __SIZE_TYPE__
call_wl (void)
{
  return wl ("abc");
}

int
main (void)
{
  struct P p = get_p ();

  if (p.x != 70 || p.y != 80)
    __builtin_abort ();
  if (sum_p () != 150)
    __builtin_abort ();
  if (call_wl () != 7)
    __builtin_abort ();
  return 0;
}
