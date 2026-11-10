/* With -mgeneral-regs-only, byte loops are vectorized with vectors held in
   X registers, which can be loaded and stored at any address.  The loops
   don't need to be versioned on the alignment of their pointers unless
   -mstrict-align is in effect.  */
/* { dg-do compile } */
/* { dg-options "-O3 -mgeneral-regs-only -fdump-tree-vect-details" } */

typedef unsigned char u8;

void
cpy8a (u8 *d, const u8 *s, long n)
{
  for (long i = 0; i < n; i++)
    d[i] = s[i];
}

void
xor8 (u8 *restrict d, const u8 *restrict s, long n)
{
  for (long i = 0; i < n; i++)
    d[i] ^= s[i];
}

__attribute__ ((target ("strict-align"))) void
xor8_strict (u8 *restrict d, const u8 *restrict s, long n)
{
  for (long i = 0; i < n; i++)
    d[i] ^= s[i];
}

/* { dg-final { scan-tree-dump-times "vectorized 1 loops" 3 "vect" } } */
/* { dg-final { scan-tree-dump-times "versioned for vectorization to enhance alignment" 1 "vect" } } */
