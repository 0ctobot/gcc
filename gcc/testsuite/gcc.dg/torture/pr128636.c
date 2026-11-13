/* { dg-do run } */
/* { dg-additional-options "-fsplit-loops" } */

/* Each loop below exits at its first exit test, so its body must run
   exactly once even though the split condition stays true for longer.  */

__attribute__((noipa)) unsigned
f (unsigned z)
{
  unsigned a = 0, i = 0;
  do
    if (i <= 3)
      a++;
  while (++i < z);
  return a;
}

__attribute__((noipa)) int
g (int z, int b)
{
  int a = 0, i = 0;
  do
    if (i <= b)
      a++;
  while (++i < z);
  return a;
}

__attribute__((noipa)) int
h (int n, int z, int b)
{
  int a = 0, i = n;
  do
    if (i >= b)
      a++;
  while (--i > z);
  return a;
}

int
main ()
{
  if (f (0) != 1 || f (1) != 1 || f (2) != 2 || f (100) != 4)
    __builtin_abort ();
  if (g (-__INT_MAX__ - 1, 3) != 1 || g (0, 3) != 1 || g (2, 3) != 2
      || g (100, 3) != 4)
    __builtin_abort ();
  if (h (20, __INT_MAX__, 10) != 1 || h (20, 20, 10) != 1
      || h (20, 18, 10) != 2 || h (20, -100, 10) != 11)
    __builtin_abort ();
  return 0;
}
