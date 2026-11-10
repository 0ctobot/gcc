/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized -mgeneral-regs-only" } */

/* The loop may become a popcount now that -mgeneral-regs-only expands it
   inline, but it must never turn into a call to __popcountdi2.  */

int PopCount (long b) {
    int c = 0;

    while (b) {
	b &= b - 1;
	c++;
    }
    return c;
}

/* { dg-final { scan-tree-dump-times "__builtin_popcount|\\.POPCOUNT" 1 "optimized" } } */
/* { dg-final { scan-assembler-not {__popcount} } } */
