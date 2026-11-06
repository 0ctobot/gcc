/* Masked stores whose addresses wrap around the address space so that two lanes
   of a vector iteration write to the same element.  The vector code performs
   one store's lanes before the other store's, which is only correct if the
   lanes of one store don't overlap the earlier lanes of the other store.  */
/* { dg-do run { target aarch64_sve_hw } } */
/* { dg-options "-O3 -fno-strict-overflow -fno-vect-cost-model -fno-version-loops-for-strides" } */

#define CONSTANT_STEP ((1L << 60) + 1)
#define SAME_REF_STEP (1L << 60)

/* Lane 2 of the first store and lane 0 of the second store meet, even though
   the difference between the accesses isn't a multiple of the step.  */
__attribute__ ((noipa)) void
constant_step (long *a, short *c, short *d, unsigned long n)
{
  for (unsigned long i = 0; i < n; i++)
    {
      if (c[i])
	a[(long) i * CONSTANT_STEP] = 42;
      if (d[i])
	a[(long) i * CONSTANT_STEP + 2] = 43;
    }
}

/* Lanes 0 and 2 of the two stores meet, even though their distance is zero.  */
__attribute__ ((noipa)) void
constant_same_ref (long *a, short *c, short *d, unsigned long n)
{
  for (unsigned long i = 0; i < n; i++)
    {
      if (d[i])
	a[(long) i * SAME_REF_STEP] = 1;
      if (c[i])
	a[(long) i * SAME_REF_STEP] = 2;
    }
}

/* Same as above but only known at run time, when the step isn't zero.  */
__attribute__ ((noipa)) void
variable_same_ref (long *a, short *c, short *d, unsigned long n, long k)
{
  for (unsigned long i = 0; i < n; i++)
    {
      if (d[i])
	a[(long) i * k] = 1;
      if (c[i])
	a[(long) i * k] = 2;
    }
}

long m[8];
short c[16], d[16];

int
main (void)
{
  /* Iteration 0 stores 43 and iteration 2 overwrites it with 42.  */
  c[2] = 1;
  d[0] = 1;
  constant_step (m, c, d, 8);
  if (m[2] != 42)
    __builtin_abort ();

  /* Iteration 0 stores 2 and iteration 2 overwrites it with 1.  */
  c[2] = 0;
  c[0] = 1;
  d[0] = 0;
  d[2] = 1;
  constant_same_ref (m, c, d, 8);
  if (m[0] != 1)
    __builtin_abort ();

  m[0] = 0;
  variable_same_ref (m, c, d, 8, SAME_REF_STEP);
  if (m[0] != 1)
    __builtin_abort ();

  return 0;
}
