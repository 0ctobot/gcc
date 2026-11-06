/* PR target/86236 */
/* { dg-do compile { target ia32 } } */
/* { dg-options "-O2 -fno-pic -mgeneral-regs-only -ffixed-edi" } */

/* A nested regparm(3) function gets its static chain in SI on a direct call,
   and its prologue pushes SI again after loading the DRAP register, so SI
   can't be the DRAP register when DI is fixed.  */

extern void g (void *);

__attribute__ ((regparm (3))) long
outer (long a, long b, long c)
{
  __attribute__ ((regparm (3), noinline)) long
  in (long x, long y, long z)
  {
    char v[64] __attribute__ ((aligned (64)));
    g (v);
    return a + x + y + z;
  }
  long (*volatile p) (long, long, long) __attribute__ ((regparm (3))) = in;
  return in (a, b, c) + p (a, b, c);
}
