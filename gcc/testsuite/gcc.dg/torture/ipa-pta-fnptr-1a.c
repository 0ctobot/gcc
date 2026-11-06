/* { dg-do compile } */

extern int x;

__attribute__ ((noinline)) int *
ext1 (void)
{
  return &x;
}

__attribute__ ((noinline)) int *
ext2 (void)
{
  return &x;
}
