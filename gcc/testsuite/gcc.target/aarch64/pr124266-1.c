/* PR target/124266 */
/* { dg-do compile } */
/* { dg-options "-O2 -fasynchronous-unwind-tables" } */

/* Exceptional returns only preserve the base PCS state, which means that the
   CFI for a vector PCS function describes V8-V15 and nothing above them, no
   matter how the registers are saved and restored.  */

#define VPCS __attribute__ ((aarch64_vector_pcs))

void base (void);
VPCS void vect (void);

/* Save V8-V23 in pairs that don't change the stack pointer.  */
VPCS void
f1 (void)
{
  base ();
}

/* Save a pair with writeback that only needs CFI for the stack adjustment.  */
VPCS void
f2 (void)
{
  asm ("" : : : "v16", "v17");
}

/* Likewise for a single register.  */
VPCS void
f3 (void)
{
  asm ("" : : : "v16");
}

/* Save a pair with writeback that has CFI for just the first register.  */
VPCS void
f4 (void)
{
  asm ("" : : : "v15", "v16");
  vect ();
}

/* { dg-final { scan-assembler-not {\.cfi_(offset|restore) 8[0-7]\M} } } */
/* { dg-final { scan-assembler-times {\.cfi_offset 72,} 1 } } */
/* { dg-final { scan-assembler-times {\.cfi_offset 73,} 1 } } */
/* { dg-final { scan-assembler-times {\.cfi_offset 74,} 1 } } */
/* { dg-final { scan-assembler-times {\.cfi_offset 75,} 1 } } */
/* { dg-final { scan-assembler-times {\.cfi_offset 76,} 1 } } */
/* { dg-final { scan-assembler-times {\.cfi_offset 77,} 1 } } */
/* { dg-final { scan-assembler-times {\.cfi_offset 78,} 1 } } */
/* { dg-final { scan-assembler-times {\.cfi_offset 79,} 2 } } */
/* { dg-final { scan-assembler-times {\.cfi_restore 72\M} 1 } } */
/* { dg-final { scan-assembler-times {\.cfi_restore 73\M} 1 } } */
/* { dg-final { scan-assembler-times {\.cfi_restore 74\M} 1 } } */
/* { dg-final { scan-assembler-times {\.cfi_restore 75\M} 1 } } */
/* { dg-final { scan-assembler-times {\.cfi_restore 76\M} 1 } } */
/* { dg-final { scan-assembler-times {\.cfi_restore 77\M} 1 } } */
/* { dg-final { scan-assembler-times {\.cfi_restore 78\M} 1 } } */
/* { dg-final { scan-assembler-times {\.cfi_restore 79\M} 2 } } */
/* { dg-final { scan-assembler-times {\.cfi_def_cfa_offset 32\M} 2 } } */
/* { dg-final { scan-assembler-times {\.cfi_def_cfa_offset 16\M} 1 } } */
/* { dg-final { scan-assembler-times {\.cfi_def_cfa_offset 0\M} 4 } } */
