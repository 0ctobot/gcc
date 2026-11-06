/* PR rtl-optimization/127776 */
/* { dg-do compile } */
/* { dg-options "-mabi=64 -mno-abicalls -fno-pic -march=r10000 -mexplicit-relocs" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Os" "-Og" "-O3" "-flto" } { "" } } */

/* Loading the address of the string literal clobbers $2 as a scratch register,
   so $2 must be loaded again for the return value.  */

__attribute__ ((noipa)) NOMIPS16 int
f (void)
{
  return 0;
}

__attribute__ ((noipa)) NOMIPS16 _Bool
g (const char **w)
{
  if (f () == 0)
    {
      *w = "b";
      return 0;
    }
  return 1;
}

/* One for f and one for g.  */
/* { dg-final { scan-assembler-times "\tmove\t\\\$2,\\\$0\n" 2 } } */
