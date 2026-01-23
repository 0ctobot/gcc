/* { dg-do compile } */
/* { dg-options "-O2 -fno-tree-dse -fno-tree-dce -mbig-endian -g" } */

/* A debug insn refers to a subreg of a pseudo that early-ra allocated, at an
   offset that doesn't map to whole FPRs.  This used to segfault.  */

typedef __attribute__((__vector_size__(16))) char U;
typedef __attribute__((__vector_size__(32))) char V;
V l;

char
foo (V o)
{
  V q = l + o;
  U r = ((union { V a; U b[2]; }) q).b[0] + ((union { V a; U b[2]; }) q).b[1];
  return r[1];
}
