/* { dg-do run } */

/* LIM hoists a load out of a loop even if the load only runs on some paths
   through the loop body.  A _Bool load can then see a byte the program never
   reads as a _Bool, which is why LIM turns the load into a full byte
   VIEW_CONVERT_EXPR load followed by a truncating cast.  That cast has to stay:
   the byte in memory can hold any value no matter the type of the memory
   reference.  */

union u { _Bool b; unsigned char c; };
struct s { int pad; union u u; };

union u g1, g2, ga[2];
_Bool flag;
int zeros[4];
int ones[4] = { 1, 1, 1, 1 };

__attribute__((noipa)) int
test_then (int n)
{
  int r = 0;

  for (int i = 0; i < n; i++)
    {
      _Bool x;

      if (zeros[i])
	x = g1.b;
      else
	x = 1;
      r += x;
    }
  return r;
}

__attribute__((noipa)) int
test_else (int n)
{
  int r = 0;

  for (int i = 0; i < n; i++)
    {
      _Bool x;

      if (ones[i])
	x = 1;
      else
	x = g2.b;
      r += x;
    }
  return r;
}

__attribute__((noipa)) int
test_array (int n)
{
  int r = 0;

  for (int i = 0; i < n; i++)
    {
      _Bool x;

      if (zeros[i])
	x = ga[1].b;
      else
	x = 1;
      r += x;
    }
  return r;
}

__attribute__((noipa)) int
test_punned (int n)
{
  int r = 0;

  for (int i = 0; i < n; i++)
    {
      _Bool x;

      if (zeros[i])
	x = flag;
      else
	x = 1;
      r += x;
    }
  return r;
}

__attribute__((noipa)) void
fill (struct s *p)
{
  p->u.c = 2;
}

__attribute__((noipa)) int
test_local (int n)
{
  struct s t;
  int r = 0;

  fill (&t);
  for (int i = 0; i < n; i++)
    {
      _Bool x;

      if (zeros[i])
	x = t.u.b;
      else
	x = 1;
      r += x;
    }
  return r;
}

int
main (void)
{
  zeros[0] = 0;
  ones[0] = 1;
  g1.c = 2;
  g2.c = 2;
  ga[1].c = 2;
  __builtin_memset (&flag, 2, sizeof (flag));

  if (test_then (4) != 4)
    __builtin_abort ();
  if (test_else (4) != 4)
    __builtin_abort ();
  if (test_array (4) != 4)
    __builtin_abort ();
  if (test_punned (4) != 4)
    __builtin_abort ();
  if (test_local (4) != 4)
    __builtin_abort ();
  return 0;
}
