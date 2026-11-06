/* { dg-options "-fno-lto" } */

extern int x;

int *
ext1 (void)
{
  return &x;
}

int *
ext2 (void)
{
  return &x;
}
