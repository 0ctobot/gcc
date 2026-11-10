/* A clear of a volatile aggregate must write each byte once, so its
   inline expansion can't finish with a store that overlaps bytes it
   already cleared.  Without the volatile, a 15-byte clear is two 8-byte
   stores at offsets 0 and 7, and a 31-byte clear is two 16-byte stores at
   offsets 0 and 15.  */
/* { dg-do compile } */
/* { dg-options "-O2" } */

struct s15 { unsigned char b[15]; };
struct s31 { unsigned char b[31]; };

void
clr15 (volatile struct s15 *d)
{
  *d = (struct s15) {};
}

void
clr31 (volatile struct s31 *d)
{
  *d = (struct s31) {};
}

/* { dg-final { scan-assembler-not {\[x0, (7|15)\]} } } */
/* { dg-final { scan-assembler {\tstrb\twzr, \[x0, 14\]} } } */
/* { dg-final { scan-assembler {\tstrb\twzr, \[x0, 30\]} } } */
/* { dg-final { scan-assembler-not {\tbl?\tmemset} } } */
