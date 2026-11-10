/* The size estimate of an asm counts only what it assembles into the
   function's own section and subsection, so each of these one-instruction
   helpers is small enough for the early inliner despite its directives.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-einline-details --param early-inlining-insns=2" } */

static inline unsigned long
f_push (void)
{
  unsigned long x;
  __asm__ ("1:\tnop %0\n"
	   "\t.pushsection .patch_table, \"a\"\n"
	   "\t.balign 8\n"
	   "\t.long 1b - .\n"
	   "\t.quad 0\n"
	   "\t.byte 4\n"
	   "\t.byte 4\n"
	   "\t.popsection\n"
	   : "=r" (x));
  return x;
}

static inline unsigned long
f_sub (void)
{
  unsigned long x;
  __asm__ (".if 1\n"
	   "661:\n\t"
	   "nop %0\n"
	   "662:\n"
	   ".subsection 1\n"
	   "663:\n\t"
	   "nop %0\n"
	   "664:\n\t"
	   ".org . - (664b-663b) + (662b-661b)\n\t"
	   ".org . - (662b-661b) + (664b-663b)\n\t"
	   ".previous\n"
	   ".endif\n"
	   : "=r" (x));
  return x;
}

static inline unsigned long
f_section (void)
{
  unsigned long x;
  __asm__ ("/* one instruction */\n"
	   "\tnop %0\n"
	   "\t.section .other, \"a\"\n"
	   "\t.long 1\n"
	   "\t.long 2\n"
	   "\t.long 3\n"
	   "\t.long 4\n"
	   "\t.previous\n"
	   : "=r" (x));
  return x;
}

unsigned long g1 (void) { return f_push () + f_sub () + f_section (); }
unsigned long g2 (void) { return f_push () ^ f_sub () ^ f_section (); }

/* { dg-final { scan-tree-dump-times "Inlining f_push/\[0-9\]+ into" 2 "einline" } } */
/* { dg-final { scan-tree-dump-times "Inlining f_sub/\[0-9\]+ into" 2 "einline" } } */
/* { dg-final { scan-tree-dump-times "Inlining f_section/\[0-9\]+ into" 2 "einline" } } */
