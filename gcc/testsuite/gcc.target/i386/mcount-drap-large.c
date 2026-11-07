/* With -fcall-saved-r10, R10 passes the check for registers mcount can
   clobber, but the large code model loads the address of mcount into R10,
   so R10 can't be the DRAP register.  */
/* { dg-do compile { target { *-*-linux* && lp64 } } } */
/* { dg-options "-O2 -pg -mno-fentry -mcmodel=large -fno-pie -fcall-saved-r10 -fno-shrink-wrap" } */

void bar (char *, char *);

void
foo (int a)
{
  char x[64] __attribute__ ((aligned (64)));
  char v[a];
  bar (x, v);
}

/* { dg-final { scan-assembler "movabsq\t\\\$mcount, %r10" } } */
/* { dg-final { scan-assembler-not "leaq\t\[^\n\]*\\(%rsp\\), %r10" } } */
