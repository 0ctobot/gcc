/* The object designator of a compound assignment to a complex part or a vector
   element must be evaluated exactly once, and the read and the write must reach
   the same object.  */
/* { dg-do run } */
/* { dg-options "-std=gnu11" } */

typedef int v4si __attribute__ ((vector_size (16)));

int calls;
_Complex int carr[4];
v4si varr[4];

__attribute__ ((noipa)) int
g (int x)
{
  calls++;
  return x;
}

#define CHECK(ncalls, cond)				\
  do							\
    {							\
      if (calls != (ncalls) || !(cond))			\
	__builtin_abort ();				\
      calls = 0;					\
      __builtin_memset (carr, 0, sizeof (carr));	\
      __builtin_memset (varr, 0, sizeof (varr));	\
    }							\
  while (0)

int
main (void)
{
  _Complex int *p = carr;
  v4si *vp = varr;

  __real__ carr[g (1)] += 1;
  CHECK (1, __real__ carr[1] == 1 && __real__ carr[0] == 0);

  __imag__ carr[g (2)] += 3;
  CHECK (1, __imag__ carr[2] == 3 && __imag__ carr[0] == 0);

  __real__ *p++ += 1;
  CHECK (0, p == carr + 1 && __real__ carr[0] == 1 && __real__ carr[1] == 0);

  p = carr + 1;
  __imag__ *p-- *= 2;
  CHECK (0, p == carr);

  __real__ carr[(int) {g (3)}] += 1;
  CHECK (1, __real__ carr[3] == 1 && __real__ carr[0] == 0);

  __real__ carr[(int) {g (1)} + 1] += 1;
  CHECK (1, __real__ carr[2] == 1 && __real__ carr[0] == 0);

  (*vp++)[1] += 1;
  CHECK (0, vp == varr + 1 && varr[0][1] == 1 && varr[1][1] == 0);

  varr[g (2)][g (3)] += 5;
  CHECK (2, varr[2][3] == 5 && varr[0][0] == 0);

  p = carr;
  (__real__ *p++)++;
  CHECK (0, p == carr + 1 && __real__ carr[0] == 1 && __real__ carr[1] == 0);

  p = carr;
  (*p++)++;
  CHECK (0, p == carr + 1 && __real__ carr[0] == 1 && __real__ carr[1] == 0);

  return 0;
}
