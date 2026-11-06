/* { dg-do run } */
/* { dg-require-effective-target int32plus } */

/* The first exit ends the loop long before v could reach -q, which takes v
   wrapping around first.  The iteration count computed for the test on v is
   thus a large value, and comparing w against its value at that count would
   make the test fire early since w has a shorter period than v.  */

__attribute__((noipa)) int
foo (int q, int u)
{
  int v = 1;
  short w = 1;
  do
    {
      v = -274202625 + v;
      if (822083585 <= q - v)
	return w;
      if (v == -q)
	return w;
      w = w - 2;
    }
  while (48 == (3 << u));
  return w;
}

int
main ()
{
  if (foo (1, 4) != -3)
    __builtin_abort ();
  return 0;
}
