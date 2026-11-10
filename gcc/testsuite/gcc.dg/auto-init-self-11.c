/* { dg-do compile } */
/* { dg-options "-ftrivial-auto-var-init=zero -fdump-tree-gimple" } */

/* A write-only asm output doesn't read the variable being initialized.  An
   input can read it directly or through its address, and an output address
   can depend on the value even if the output itself is write-only.  */

int
reg_output (void)
{
  volatile int reg = ({ __asm__ volatile ("" : "=r" (reg)); 2; });
  return reg;
}

int
mem_output (void)
{
  volatile int mem = ({ __asm__ volatile ("" : "=m" (mem)); 2; });
  return mem;
}

int
inout (void)
{
  int rw = ({ __asm__ volatile ("" : "+r" (rw)); 2; });
  return rw;
}

int
matched_input (void)
{
  int tied = ({ __asm__ volatile ("" : "=r" (tied) : "0" (tied)); 2; });
  return tied;
}

int
uses_address (void)
{
  int addr = ({ __asm__ volatile ("" : "=r" (addr) : "r" (&addr)); 2; });
  return addr;
}

int
output_index (int *p)
{
  int idx = ({ __asm__ volatile ("" : "=m" (p[idx])); 2; });
  return idx;
}

struct S { int a[2], n; };

int
member_index (void)
{
  struct S s = ({ __asm__ volatile ("" : "=m" (s.a[s.n]));
		  (struct S) { { 0, 0 }, 1 }; });
  return s.n;
}

/* { dg-final { scan-tree-dump-not "\"reg\"" "gimple" } } */
/* { dg-final { scan-tree-dump-not "\"mem\"" "gimple" } } */
/* { dg-final { scan-tree-dump-times "\"rw\"" 1 "gimple" } } */
/* { dg-final { scan-tree-dump-times "\"tied\"" 1 "gimple" } } */
/* { dg-final { scan-tree-dump-times "\"addr\"" 1 "gimple" } } */
/* { dg-final { scan-tree-dump-times "\"idx\"" 1 "gimple" } } */
/* { dg-final { scan-tree-dump-times "\"s\"" 1 "gimple" } } */
