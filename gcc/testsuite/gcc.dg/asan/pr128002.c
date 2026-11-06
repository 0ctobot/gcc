/* PR sanitizer/128002 */
/* { dg-do run } */
/* { dg-require-effective-target trampolines } */

__attribute__((noipa)) int
f (int x, int y)
{
  int *p = &x;
  int *q = &y;
  int get (void) { return x; }
  return *p + *q + get ();
}

int
main (void)
{
  if (f (1, 2) != 4)
    __builtin_abort ();
  return 0;
}
