/* PR target/124266 */
/* { dg-do compile } */
/* { dg-options "-O2 -fasynchronous-unwind-tables" } */

/* V15 and V16 are saved and restored around the call to g by separate
   shrink-wrapping, which emits single loads and stores that pair fusion then
   merges.  Only V15 has CFI, and it needs to survive the fusion since the
   code after the if-else is also reached by a path that doesn't save V15.  */

#define VPCS __attribute__ ((aarch64_vector_pcs))

VPCS void g (void);
VPCS void k (void);
VPCS void j (void);

extern int x;

VPCS int
f1 (void)
{
  if (__builtin_expect (x, 1))
    {
      asm ("" : : : "v15", "v16");
      g ();
    }
  else
    k ();
  j ();
  j ();
  j ();
  j ();
  j ();
  j ();
  j ();
  j ();
  return 0;
}

/* { dg-final { scan-assembler {stp\tq15, q16, \[sp, [0-9]+\]\n\t\.cfi_offset 79, } } } */
/* { dg-final { scan-assembler {ldp\tq15, q16, \[sp, [0-9]+\]\n\t\.cfi_restore 79\n} } } */
/* { dg-final { scan-assembler-not {\.cfi_(offset|restore) 80\M} } } */
