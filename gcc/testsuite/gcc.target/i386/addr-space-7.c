/* The vectorizer's data-reference groups look through integer casts of the
   pointer, so these accesses appear to be neighbors at one base address even
   though they use different segments.  Merging them would access the second
   byte through the first access's segment.  */
/* { dg-do compile { target lp64 } } */
/* { dg-options "-O2 -ftree-slp-vectorize" } */

#define GS(p) (*(char __seg_gs *) (p))
#define FS(p) (*(char __seg_fs *) (p))
#define GEN(p) (*(char *) (p))

void
store_gs_gen (unsigned long v)
{
  GS (v) = 3;
  GEN (v + 1) = 9;
}

void
store_gen_gs (unsigned long v)
{
  GEN (v) = 4;
  GS (v + 1) = 10;
}

void
store_gs_fs (unsigned long v)
{
  GS (v) = 5;
  FS (v + 1) = 11;
}

void
load_gs_gen (unsigned long v, char *restrict r)
{
  char a = GS (v), b = GEN (v + 1);
  r[0] = a;
  r[1] = b;
}

void
load_fs_gs (unsigned long v, char *restrict r)
{
  char a = FS (v), b = GS (v + 1);
  r[0] = a;
  r[1] = b;
}

/* Accesses in one segment can still be merged.  */
struct S { char a, b; };

void
store_gs_gs (struct S __seg_gs *p)
{
  p->a = 1;
  p->b = 2;
}

/* { dg-final { scan-assembler {movb\s+\$3, %gs:\(%rdi\)} } } */
/* { dg-final { scan-assembler {movb\s+\$9, 1\(%rdi\)} } } */
/* { dg-final { scan-assembler {movb\s+\$4, \(%rdi\)} } } */
/* { dg-final { scan-assembler {movb\s+\$10, %gs:1\(%rdi\)} } } */
/* { dg-final { scan-assembler {movb\s+\$5, %gs:\(%rdi\)} } } */
/* { dg-final { scan-assembler {movb\s+\$11, %fs:1\(%rdi\)} } } */
/* { dg-final { scan-assembler-times {movzbl\s+%gs:\(%rdi\)} 1 } } */
/* { dg-final { scan-assembler-times {movzbl\s+1\(%rdi\)} 1 } } */
/* { dg-final { scan-assembler-times {movzbl\s+%fs:\(%rdi\)} 1 } } */
/* { dg-final { scan-assembler-times {movzbl\s+%gs:1\(%rdi\)} 1 } } */
/* { dg-final { scan-assembler-times {movw\s+[^,]+, %gs:\(%rdi\)} 1 } } */
