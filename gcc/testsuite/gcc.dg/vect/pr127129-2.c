/* { dg-do compile } */
/* { dg-require-effective-target vect_int } */
/* { dg-require-effective-target lp64 } */
/* { dg-additional-options "-fdump-tree-vect-details" } */

/* This loop runs 2^32 times if N is 0, so N in an unsigned int wraps when the
   number of latch executions is converted to a number of iterations.  Taking
   1 from that number in sizetype would then give a segment length of -1 times
   the step, which breaks the run-time alias test.  */

void
f (const char *b, short *a, unsigned int n)
{
  unsigned int i = 0;

  do
    *a++ = *b++;
  while (++i != n);
}

/* { dg-final { scan-tree-dump-not "segment length:\[^\n\]*18446744073709551615" "vect" } } */
