/* { dg-options "-fno-lto" } */

extern int g;

void
ext1 (void)
{
  g = 2;
}

void
ext2 (void)
{
  g = 3;
}
