/* PR target/86236 */
/* { dg-do compile { target lp64 } } */
/* { dg-options "-O2 -fno-pic -ffixed-r8 -ffixed-r9 -ffixed-r10 -ffixed-r11 -ffixed-r12 -ffixed-r13 -ffixed-r14 -ffixed-r15 -ffixed-rbx -ffixed-rcx -ffixed-rdi -ffixed-rsi" } */

/* DX is the last register left, but this function returns part of its value
   there and the epilogue would overwrite it.  */

char *volatile p;
int *volatile q;

__int128
f (void)
{
  char *a = __builtin_alloca (32);
  int x __attribute__ ((aligned (64)));
  p = a;
  q = &x;
  return x;
} /* { dg-message "no register is available" } */
