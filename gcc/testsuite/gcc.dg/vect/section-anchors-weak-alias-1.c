/* { dg-require-effective-target section_anchors } */
/* { dg-require-weak "" } */
/* { dg-require-alias "" } */
/* { dg-additional-options "-fsection-anchors -fdump-ipa-increase_alignment-details" } */

/* A strong alias of a weak array names the storage of the weak definition,
   whose alignment can't be increased since another unit may replace that
   definition.  Increasing the alignment of the alias alone would make code
   using the alias assume an alignment that the storage doesn't have.  */

#include "tree-vect.h"

#define N 64

__attribute__ ((weak)) int w[N] = { 1 };
extern int a[N] __attribute__ ((alias ("w")));

/* Defined after w so that it is laid out in front of w, which then lands
   8 bytes past a 16-byte boundary unless its alignment is increased.  */
__attribute__ ((used, aligned (16))) long long pad = 1;

__attribute__ ((noipa)) __UINTPTR_TYPE__
opaque (__UINTPTR_TYPE__ x)
{
  return x;
}

__attribute__ ((noipa)) void
check (__UINTPTR_TYPE__ x, __UINTPTR_TYPE__ y)
{
  if (x != y)
    abort ();
}

__attribute__ ((noipa)) int
sum (void)
{
  int s = 0;
  for (int i = 0; i < N; i++)
    s += a[i];
  return s;
}

int
main (void)
{
  check_vect ();
  /* The first argument becomes the constant 0 if the alignment of a was
     increased to 16 bytes.  */
  check ((__UINTPTR_TYPE__) a & 15, opaque ((__UINTPTR_TYPE__) a) & 15);
  if (sum () != 1)
    abort ();
  return 0;
}

/* { dg-final { scan-ipa-dump-not "Increasing alignment of decl" "increase_alignment" } } */
