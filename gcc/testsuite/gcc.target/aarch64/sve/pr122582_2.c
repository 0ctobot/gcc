/* PR target/122582 */
/* { dg-do run { target aarch64_sve256_hw } } */
/* { dg-options "-O3 -msve-vector-bits=256 -fno-vect-cost-model" } */

#define N 87
#define SIZE (2 * 6 * N + 8)

typedef signed char sc;

static sc a[SIZE], b[SIZE];

#define BODY(STEP, BARRIER)			\
  for (int i = 0; i < N; i++)			\
    {						\
      BARRIER;					\
      p[i * (STEP) + 0] += 1;			\
      p[i * (STEP) + 1] += 2;			\
      p[i * (STEP) + 2] += 4;			\
      p[i * (STEP) + 3] += 8;			\
    }

void __attribute__ ((noipa))
runtime_step (sc *p, int step)
{
  BODY (step, )
}

void __attribute__ ((noipa))
negative_step (sc *p)
{
  BODY (-6, )
}

void __attribute__ ((noipa))
positive_step (sc *p)
{
  BODY (6, )
}

static void __attribute__ ((noipa))
reference (sc *p, int step)
{
  BODY (step, __asm__ volatile ("" : : : "memory"))
}

static void
check (void)
{
  for (int i = 0; i < SIZE; i++)
    if (a[i] != b[i])
      __builtin_abort ();
}

int
main (void)
{
  for (int step = -6; step <= 6; step++)
    {
      int start = step < 0 ? -step * N : 0;
      runtime_step (a + start, step);
      reference (b + start, step);
      check ();
    }

  negative_step (a + 6 * N);
  reference (b + 6 * N, -6);
  check ();

  positive_step (a);
  reference (b, 6);
  check ();
  return 0;
}
