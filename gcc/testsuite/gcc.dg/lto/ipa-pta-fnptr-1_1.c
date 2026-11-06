extern int g;

__attribute__ ((noinline)) void
ext1 (void)
{
  g = 2;
}

__attribute__ ((noinline)) void
ext2 (void)
{
  g = 3;
}
