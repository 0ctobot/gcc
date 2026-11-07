/* The row step K is only known at run time, but the guard ahead of the loop
   keeps it between 1 and 100.  That's enough to tell the iterations apart in
   the first dimension, since K can't be zero, so the loop needs no run-time
   alias check.  */
/* { dg-do compile } */
/* { dg-require-effective-target vect_int } */
/* { dg-additional-options "-O3" } */

void
guarded (int (*a)[8], long n, long k)
{
  if (k < 1 || k > 100)
    return;
  for (long i = 0; i < n; i++)
    a[k * i][i + 2] = a[k * i][i] * 2 + 1;
}

/* { dg-final { scan-tree-dump "LOOP VECTORIZED" "vect" { target { aarch64*-*-* x86_64-*-* } } } } */
/* { dg-final { scan-tree-dump-not "versioning for alias required" "vect" } } */
