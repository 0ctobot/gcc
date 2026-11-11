/* { dg-do compile } */
/* { dg-options "-O2 -mgeneral-regs-only -mtune=cortex-a725" } */

/* A vector of four 32-bit elements built in general registers occupies
   two X registers, and each of them is zeroed before its two BFIs.
   Building four such vectors from the table lookups takes more
   instructions than the EORs it saves, so BB SLP must keep the scalar
   code.  */

typedef unsigned int u32;

void
last_round (unsigned char *__restrict out, const u32 *__restrict k,
	    const u32 (*__restrict tab)[256], u32 a, u32 b, u32 c, u32 d)
{
  u32 x0 = (tab[0][a & 255] ^ tab[1][b >> 8 & 255]
	    ^ tab[2][c >> 16 & 255] ^ tab[3][d >> 24] ^ k[0]);
  u32 x1 = (tab[0][b & 255] ^ tab[1][c >> 8 & 255]
	    ^ tab[2][d >> 16 & 255] ^ tab[3][a >> 24] ^ k[1]);
  u32 x2 = (tab[0][c & 255] ^ tab[1][d >> 8 & 255]
	    ^ tab[2][a >> 16 & 255] ^ tab[3][b >> 24] ^ k[2]);
  u32 x3 = (tab[0][d & 255] ^ tab[1][a >> 8 & 255]
	    ^ tab[2][b >> 16 & 255] ^ tab[3][c >> 24] ^ k[3]);
  __builtin_memcpy (out, &x0, 4);
  __builtin_memcpy (out + 4, &x1, 4);
  __builtin_memcpy (out + 8, &x2, 4);
  __builtin_memcpy (out + 12, &x3, 4);
}

/* { dg-final { scan-assembler-not {\tbfi\t} } } */
/* { dg-final { scan-assembler-times {\teor\t} 16 } } */
