/* A store through a pointer to a weakref's target must be seen by a direct read
   through the weakref, even though the weakref is static and this unit knows
   nothing about the target.  */
/* { dg-lto-do run } */
/* { dg-require-weak "" } */
/* { dg-require-alias "" } */
/* { dg-lto-options {{-O2 -flto}} } */

static int w __attribute__ ((weakref ("alias_weakref_target")));

__attribute__ ((noinline)) int
test (int *p)
{
  w = 1;
  *p = 2;
  return w;
}
