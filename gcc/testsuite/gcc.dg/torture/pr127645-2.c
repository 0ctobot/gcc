/* PR tree-optimization/127645 */
/* { dg-do run } */
/* { dg-additional-options "-fno-tree-sra" } */

/* The limited conditional store elimination loads the memory on the path
   that skips the store.  The byte there doesn't have to be a valid _Bool.  */

union u { unsigned char f1; _Bool f2; };

__attribute__ ((noipa)) int
f (int c, unsigned char v)
{
  union u u;

  u.f1 = v;
  if (c)
    u.f2 = 1;
  return u.f1;
}

__attribute__ ((noipa)) int
g (int c, unsigned char v)
{
  union u u;

  u.f1 = v;
  if (c)
    u.f2 = 1;
  return u.f1 == v;
}

int
main (void)
{
  if (f (0, 45) != 45 || f (1, 45) != 1)
    __builtin_abort ();
  if (f (0, 2) != 2)
    __builtin_abort ();
  if (!g (0, 45) || !g (0, 254))
    __builtin_abort ();
  return 0;
}
