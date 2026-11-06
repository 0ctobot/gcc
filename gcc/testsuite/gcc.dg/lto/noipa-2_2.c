extern void abort (void);

__attribute__ ((noipa)) int z (int);

int
other (void)
{
  int x = z (5);
  if (__builtin_constant_p (x))
    abort ();
  return x;
}
