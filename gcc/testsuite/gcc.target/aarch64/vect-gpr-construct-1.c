/* { dg-do compile } */
/* { dg-options "-O2 -mgeneral-regs-only" } */

/* Building a vector in a general register takes a move and one BFI per
   element, so BB SLP must not replace these narrow stores with a BFI chain
   and one wider store.  */

typedef short int16_t;
typedef unsigned short uint16_t;

void
dup_2_aligned (int16_t *x, int16_t y)
{
  x = __builtin_assume_aligned (x, 4);
  x[0] = y;
  x[1] = y;
}

char a[100] __attribute__ ((aligned (4)));

void
dup_4_aligned (int x, int y)
{
  char *p = a + (y & ~3);
  p[0] = x;
  p[1] = x;
  p[2] = x;
  p[3] = x;
}

void
dup_2 (int16_t *x, int16_t y)
{
  x[0] = y;
  x[1] = y;
}

void
dup_4 (int16_t *x, int16_t y)
{
  x[0] = y;
  x[1] = y;
  x[2] = y;
  x[3] = y;
}

struct s { uint16_t a, b, c, d; };

void
cons_4 (struct s *p, uint16_t v, uint16_t w)
{
  p->a = v;
  p->b = w;
  p->c = v;
  p->d = w;
}

struct s8 { uint16_t a, b, c, d; } __attribute__ ((aligned (8)));

void
cons_4_aligned (struct s8 *p, uint16_t v, uint16_t w)
{
  p->a = v;
  p->b = w;
  p->c = v;
  p->d = w;
}

/* { dg-final { scan-assembler-not {\tbfi\t} } } */
/* { dg-final { scan-assembler-times {\tstrh\t} 16 } } */
/* { dg-final { scan-assembler-times {\tstrb\t} 4 } } */
