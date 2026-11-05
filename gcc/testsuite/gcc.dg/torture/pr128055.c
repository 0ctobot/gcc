/* { dg-do run } */
/* { dg-require-alias "" } */

int a = 0;
extern int b __attribute__((alias("a")));

__attribute__((noipa)) int
f1 (void)
{
  a = 1;
  int x = b;
  a = 2;
  return x;
}

__attribute__((noipa)) int
f2 (int c)
{
  a = 1;
  int x;
  if (c)
    {
      __asm__ volatile ("" : "+r"(c));
      x = b + 1;
    }
  else
    {
      x = b + 2;
    }
  a = 2;
  return x;
}

int
main (void)
{
  if (f1 () != 1)
    __builtin_abort ();
  if (f2 (1) != 2)
    __builtin_abort ();
  if (f2 (0) != 3)
    __builtin_abort ();
  return 0;
}
