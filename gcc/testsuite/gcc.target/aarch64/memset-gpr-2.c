/* At -Os without SIMD, memsets of up to 48 bytes are expanded inline with
   stores from general registers, and larger ones call memset.  */
/* { dg-do compile } */
/* { dg-options "-Os -mgeneral-regs-only" } */

void
c48 (char *p)
{
  __builtin_memset (p, 0x5a, 48);
}

void
c49 (char *p)
{
  __builtin_memset (p, 0x5a, 49);
}

/* { dg-final { scan-assembler-times {\tstp\tx[0-9]+, x[0-9]+, \[x0} 3 } } */
/* { dg-final { scan-assembler-times {\tbl?\tmemset} 1 } } */
