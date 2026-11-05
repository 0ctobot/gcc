/* { dg-do compile } */
/* { dg-options "-O2 -Wno-psabi --param=aarch64-ldp-policy=aligned --param=aarch64-stp-policy=aligned -fdump-rtl-expand" } */

/* V is coalesced with an anonymous copy of G1 whose stack slot is only 16 byte
   aligned.  V's own alignment says nothing about that slot, so the MEMs must
   not claim it, and the aligned policy must not form Q register pairs from
   them.  */

extern long g1 __attribute__ ((vector_size (128)));
extern long g2 __attribute__ ((vector_size (128)));
extern long bar (void) __attribute__ ((vector_size (128)));
extern void sink (long __attribute__ ((vector_size (128))));
extern void sa (long *);

void
f (int n)
{
  long a[1];
  sa (a);
  long s __attribute__ ((vector_size (128))) = g1 + g2;
  long v __attribute__ ((vector_size (128), aligned (64))) = g1;
  if (__builtin_expect (n, 0))
    v = bar ();
  sink (s);
  sink (v);
  sa (a);
}

/* { dg-final { scan-rtl-dump-not "v\\+\[0-9\]+ S\[0-9\]+ A(256|512)\\\]" "expand" } } */
/* { dg-final { scan-rtl-dump "v\\+\[0-9\]+ S\[0-9\]+ A128\\\]" "expand" } } */
/* { dg-final { scan-assembler-not "ldp\tq" } } */
/* { dg-final { scan-assembler-not "stp\tq" } } */
