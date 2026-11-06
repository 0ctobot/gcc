int
y (int v)
{
  return v + 1;
}

extern int z (int) __attribute__ ((alias ("y")));
