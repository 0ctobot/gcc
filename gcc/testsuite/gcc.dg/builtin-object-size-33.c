/* Asking whether a step repeats for every step in a long chain of pointer
   steps takes time cubic in the length of the chain, unless each step's answer
   is only worked out once.  A chain this long must not take long to compile.  */
/* { dg-do compile } */
/* { dg-options "-O2 --param=ssa-name-def-chain-limit=8000" } */
/* { dg-timeout 20 } */

char buf[9000];

#define S1 if (k) p += 2; else p += 3;
#define S10 S1 S1 S1 S1 S1 S1 S1 S1 S1 S1
#define S100 S10 S10 S10 S10 S10 S10 S10 S10 S10 S10

unsigned long
f (int k)
{
  char *p = buf + 4000;
  p += -1;
  S100 S100 S100 S100 S100 S100 S100 S100 S100
  S100 S100 S100 S100 S100 S100 S100 S100 S100
  return __builtin_object_size (p, 1);
}
