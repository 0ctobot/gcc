/* PR tree-optimization/127345 */
/* { dg-do run { target bitint } } */
/* { dg-options "-std=c23" } */

int a;
long b;

int
main ()
{
  long d = 0;
  struct { _BitInt (3) c; } e;
  if (!b)
    {
      for (; a; d = 1)
	;
      e.c = d - 1;
    }
  if ((unsigned long long) (long long) e.c != -1ULL)
    __builtin_abort ();
}
