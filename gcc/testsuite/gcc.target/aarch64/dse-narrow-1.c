/* { dg-do compile } */
/* { dg-options "-O2 -fno-stack-protector -fdump-rtl-dse1-details" } */
/* { dg-require-effective-target lp64 } */
/* { dg-final { check-function-bodies "**" "" "" } } */

/* When a structure is cleared and some of its members are then set, a
   member store can overwrite one word of a two-word store of the clear.
   RTL DSE then stores just the other word, which pairs up with the
   member store.  */

struct xa_state
{
  void *xa;
  unsigned long xa_index;
  unsigned char xa_shift, xa_sibs, xa_offset, xa_pad;
  void *xa_node, *xa_alloc, *xa_update, *xa_lru;
};

void *xas_store (struct xa_state *, void *);

/* The clear of xa_shift..xa_node is one store pair, and the store to
   xa_node overwrites its second word.  */

void *
xa_erase_like (void *xa, unsigned long index)
{
  struct xa_state xas = { .xa = xa, .xa_index = index,
			  .xa_node = (void *) 3 };
  return xas_store (&xas, (void *) 0);
}

struct s6 { unsigned long a, b, c, d, e, f; };

/*
** set_b_e:
**	stp	xzr, x1, \[x0\]
**	stp	xzr, xzr, \[x0, 16\]
**	stp	x2, xzr, \[x0, 32\]
**	ret
*/
void
set_b_e (struct s6 *p, unsigned long x, unsigned long y)
{
  *p = (struct s6) { .b = x, .e = y };
}

/* { dg-final { scan-rtl-dump-times "Narrowing insn" 3 "dse1" } } */
/* { dg-final { scan-assembler {\tstp\txzr, x[0-9]+, \[sp, [0-9]+\]} } } */
/* { dg-final { scan-assembler-not {\tstr\tx[0-9]+, \[sp} } } */
