/* { dg-do run } */
/* { dg-options "-O3 -lm" } */

/* The vectorizer recognizes pow (x, 2.0) as the multiply x * x.  When that call
   takes a load that's invariant in the outer loop of an outer-loop vectorized
   nest, the aarch64 vector cost hook checks whether the load can become a
   by-lane operand.  It used to take the tree code from the multiply but read
   the operand from the call, which crashed the compiler.  */

extern double pow (double, double) __attribute__ ((__const__, __nothrow__));
extern float powf (float, float) __attribute__ ((__const__, __nothrow__));

#define N 40
#define M 1024

double in[N + M], coeff[M], out[N];
float inf[N + M], coefff[M], outf[N];

__attribute__ ((noipa)) void
foo (void)
{
  for (int i = 0; i < N; i++)
    {
      double diff = 0.0;
      for (int j = 0; j < M; j++)
	diff += in[j + i] * pow (coeff[j], 2.0);
      out[i] = diff;
    }
}

__attribute__ ((noipa)) void
foof (void)
{
  for (int i = 0; i < N; i++)
    {
      float diff = 0.0f;
      for (int j = 0; j < M; j++)
	diff += inf[j + i] * powf (coefff[j], 2.0f);
      outf[i] = diff;
    }
}

/* The largest sum is 1024 * 7 * 5 * 5, so every intermediate sum is an integer
   that both float and double represent exactly.  */

int
main (void)
{
#pragma GCC novector
  for (int i = 0; i < N + M; i++)
    {
      in[i] = i % 7 + 1;
      inf[i] = i % 7 + 1;
    }

#pragma GCC novector
  for (int j = 0; j < M; j++)
    {
      coeff[j] = j % 5 + 1;
      coefff[j] = j % 5 + 1;
    }

  foo ();
  foof ();

#pragma GCC novector
  for (int i = 0; i < N; i++)
    {
      int expect = 0;

#pragma GCC novector
      for (int j = 0; j < M; j++)
	expect += ((j + i) % 7 + 1) * (j % 5 + 1) * (j % 5 + 1);

      if (out[i] != expect || outf[i] != expect)
	__builtin_abort ();
    }

  return 0;
}
