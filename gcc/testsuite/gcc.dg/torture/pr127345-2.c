/* PR tree-optimization/127345 */
/* { dg-do run { target bitint } } */
/* { dg-options "-std=c23" } */

union U { _BitInt (8) a; struct { _BitInt (3) c; } s; };

/* The upper bits of U.A are unknown after a store to U.S.C, so nothing about
   Y can be concluded from U.A not being -1.  */

[[gnu::noipa]] int
f (unsigned _BitInt (3) x)
{
  union U u;
  _BitInt (3) y = (_BitInt (3)) x;
  u.s.c = y;
  if (u.a == -1)
    return 1;
  if (y == -1)
    return 2;
  return 3;
}

int
main ()
{
  if (f (7) == 3)
    __builtin_abort ();
}
