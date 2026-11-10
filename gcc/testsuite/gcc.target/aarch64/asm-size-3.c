/* The size estimate of an asm counts only what it assembles into the
   function's own section and subsection, so each of these one-instruction
   helpers, written the way the arm64 kernel writes them, is small enough
   for the early inliner despite its directives, ';' separators, '//'
   comments and string contents.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-einline-details --param early-inlining-insns=2" } */

/* An ALTERNATIVE() whose replacement goes into subsection 1.  */
static inline unsigned long
f_alt (void)
{
  unsigned long x;
  __asm__ (".if 1 == 1\n"
	   "661:\n\t"
	   "mrs %0, tpidr_el1\n"
	   "662:\n"
	   ".pushsection .altinstructions,\"a\"\n"
	   " .word 661b - .\n"
	   " .word 663f - .\n"
	   " .hword 32\n"
	   " .byte 662b-661b\n"
	   " .byte 664f-663f\n"
	   ".popsection\n"
	   ".subsection 1\n"
	   "663:\n\t"
	   "mrs %0, tpidr_el2\n"
	   "664:\n\t"
	   ".org\t. - (664b-663b) + (662b-661b)\n\t"
	   ".org\t. - (662b-661b) + (664b-663b)\n\t"
	   ".previous\n"
	   ".endif\n"
	   : "=r" (x));
  return x;
}

/* A BUG() entry, with its statements separated by ';'.  */
static inline void
f_bug (void)
{
  __asm__ __volatile__ ("\t.pushsection __bug_table,\"aw\"; .align 2; "
			"14470: .long 14471f - .; .short 0; .align 2; "
			".popsection; 14471: brk 0x800");
}

/* A '//' comment, including one that holds a ';'.  */
static inline unsigned long
f_comment (void)
{
  unsigned long x;
  __asm__ ("\t// Read the virtual counter.  A read that isn't\n"
	   "\t// ordered can be satisfied early; the caller\n"
	   "\t// orders it if it needs to.\n"
	   "\tmrs\t%0, cntvct_el0\t// one instruction\n"
	   : "=r" (x));
  return x;
}

/* An .asciz string in another section that holds ';' and a "\n" escape.  */
static inline unsigned long
f_str (void)
{
  unsigned long x;
  __asm__ ("\t.pushsection .rodata.str1.1,\"aMS\",@progbits,1\n"
	   "1:\t.asciz \"state; cpu; flags; count\\n\"\n"
	   "\t.popsection\n"
	   "\tadr\t%0, 1b\n"
	   : "=r" (x));
  return x;
}

unsigned long
g1 (int c)
{
  if (c)
    f_bug ();
  return f_alt () + f_comment () + f_str ();
}

unsigned long
g2 (int c)
{
  if (c)
    f_bug ();
  return f_alt () ^ f_comment () ^ f_str ();
}

/* { dg-final { scan-tree-dump-times "Inlining f_alt/\[0-9\]+ into" 2 "einline" } } */
/* { dg-final { scan-tree-dump-times "Inlining f_bug/\[0-9\]+ into" 2 "einline" } } */
/* { dg-final { scan-tree-dump-times "Inlining f_comment/\[0-9\]+ into" 2 "einline" } } */
/* { dg-final { scan-tree-dump-times "Inlining f_str/\[0-9\]+ into" 2 "einline" } } */
