/* { dg-do compile } */
/* { dg-options "-O2 -fno-strict-aliasing -mgeneral-regs-only" } */
/* { dg-final { check-function-bodies "**" "" "" } } */

/* DSE trims the dead ends of a clear through a pointer, but only by whole
   words, so the clear stays a few aligned stores instead of becoming an
   odd-sized store at an odd offset or a memset call.  */

struct in { char c; char pad[7]; long q[3]; };
struct out { long h; struct in i; long t; };
struct it { unsigned char type; _Bool nofault, src; unsigned long off;
	    void *p; unsigned long cnt; unsigned long nr; };
struct tl { long b; char d[7]; char c; };
struct hd { char c; char d[7]; long b; };
struct s6 { unsigned long a, b, c, d, e, f; };

/*
** t3:
**	stp	xzr, xzr, \[x0\]
**	mov	(w[0-9]+), 65537
**	str	\1, \[x0\]
**	stp	x1, x2, \[x0, 16\]
**	str	xzr, \[x0, 32\]
**	ret
*/
void
t3 (struct it *i, void *b, unsigned long n)
{
  *i = (struct it){ .type = 1, .src = 1, .p = b, .cnt = n };
}

/*
** t8:
**	stp	xzr, xzr, \[x0, 8\]
**	stp	xzr, x1, \[x0, 24\]
**	mov	(w[0-9]+), 3
**	strb	\1, \[x0, 8\]
**	ret
*/
void
t8 (struct out *o, long v)
{
  o->i = (struct in){};
  o->i.c = 3;
  o->i.q[2] = v;
}

/*
** u2:
**	stp	xzr, xzr, \[x0\]
**	mov	(w[0-9]+), 1
**	strb	\1, \[x0, 15\]
**	ret
*/
void
u2 (struct tl *p)
{
  __builtin_memset (p, 0, 16);
  p->c = 1;
}

/*
** u5:
**	stp	xzr, xzr, \[x0\]
**	mov	(w[0-9]+), 1
**	strb	\1, \[x0, 15\]
**	ret
*/
void
u5 (struct tl *p)
{
  *p = (struct tl){};
  p->c = 1;
}

/*
** h1:
**	stp	xzr, xzr, \[x0\]
**	mov	(w[0-9]+), 1
**	strb	\1, \[x0\]
**	ret
*/
void
h1 (struct hd *p)
{
  __builtin_memset (p, 0, 16);
  p->c = 1;
}

/*
** h2:
**	stp	xzr, xzr, \[x0\]
**	mov	(w[0-9]+), 513
**	strh	\1, \[x0\]
**	ret
*/
void
h2 (struct hd *p)
{
  __builtin_memset (p, 0, 16);
  p->c = 1;
  p->d[0] = 2;
}

/*
** h3:
**	stp	xzr, xzr, \[x0\]
**	mov	(w[0-9]+), 1
**	strb	\1, \[x0\]
**	ret
*/
void
h3 (struct hd *p)
{
  *p = (struct hd){};
  p->c = 1;
}

/*
** m_tail3:
**	stp	xzr, xzr, \[x0\]
**	stp	xzr, x1, \[x0, 16\]
**	stp	x1, x1, \[x0, 32\]
**	ret
*/
void
m_tail3 (struct s6 *p, unsigned long v)
{
  __builtin_memset (p, 0, sizeof *p);
  p->d = v;
  p->e = v;
  p->f = v;
}

/*
** c_tail3:
**	stp	xzr, xzr, \[x0\]
**	stp	xzr, x1, \[x0, 16\]
**	stp	x1, x1, \[x0, 32\]
**	ret
*/
void
c_tail3 (struct s6 *p, unsigned long v)
{
  *p = (struct s6){};
  p->d = v;
  p->e = v;
  p->f = v;
}

/* { dg-final { scan-assembler-not "bl\tmemset" } } */
