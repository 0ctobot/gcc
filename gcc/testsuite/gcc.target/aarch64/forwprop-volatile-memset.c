/* A volatile byte store followed by a memset that covers it is still
   performed as a one-byte store.  */
/* { dg-do compile } */
/* { dg-options "-O2 -mgeneral-regs-only" } */

void
store_then_fill (char *p)
{
  *(volatile char *) p = 'a';
  __builtin_memset (p, ' ', 4);
}

/* A plain byte store still merges into one 32-bit store.  */
void
plain_store_then_pad (char *p)
{
  *p = 'a';
  __builtin_memset (p + 1, ' ', 3);
}

/* { dg-final { scan-assembler-times "\tstrb\tw\[0-9\]+, \\\[x0\\\]\n" 1 } } */
/* { dg-final { scan-assembler-times "\tstr\tw\[0-9\]+, \\\[x0\\\]\n" 2 } } */
/* { dg-final { scan-assembler-not "\tstrh\t" } } */
