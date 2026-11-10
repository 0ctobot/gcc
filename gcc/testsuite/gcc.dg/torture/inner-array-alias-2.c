/* A variable index into a flexible, [1] or [0] array that isn't the last
   member of its structure, or into a [0] marker, reaches the bytes that
   follow the array.  Byte stores, memset, memcpy and structure copies of
   the enclosing object must all be seen to overlap such an access.  */
/* { dg-do run } */
/* { dg-options "-std=gnu11 -fno-strict-aliasing -Wno-stringop-overflow" } */

#define NI __attribute__((noipa))
typedef unsigned char u8;

struct hf { u8 n; u8 d[]; };
struct h1 { u8 n; u8 d[1]; };
struct h0 { u8 n; u8 d[0]; };
struct WF { struct hf h; u8 st[8]; };
struct W1 { struct h1 h; u8 st[8]; };
struct W0 { struct h0 h; u8 st[8]; };
struct M0 { u8 n; u8 mark[0]; u8 a; u8 b; u8 c; u8 e; };

NI void
clob (void *p, int v)
{
  __builtin_memset (p, v, 6);
}

/* I is the variable index, and OFF is the byte offset in the enclosing
   structure that ARR[I] lands on.  Every function returns 7.  */
#define T(N, W, ARR)							\
NI int raw_##N (struct W *p, int i, int off)				\
{ p->ARR[i] = 1; ((u8 *) p)[off] = 7; return p->ARR[i]; }		\
NI int raw2_##N (struct W *p, int i, int off)				\
{ ((u8 *) p)[off] = 1; p->ARR[i] = 7; return ((u8 *) p)[off]; }	\
NI int ms_##N (struct W *p, int i)					\
{ p->ARR[i] = 1; __builtin_memset (p, 7, sizeof *p); return p->ARR[i]; } \
NI int ms2_##N (struct W *p, int i, int off)				\
{									\
  u8 t[sizeof *p];							\
  __builtin_memset (p, 1, sizeof *p);					\
  p->ARR[i] = 7;							\
  __builtin_memcpy (t, p, sizeof *p);					\
  return t[off];							\
}									\
NI int cp_##N (struct W *p, struct W *q, int i)				\
{ p->ARR[i] = 1; *p = *q; return p->ARR[i]; }				\
NI int cp2_##N (struct W *p, struct W *q, int i, int off)		\
{									\
  __builtin_memset (q, 1, sizeof *q);					\
  q->ARR[i] = 7;							\
  *p = *q;								\
  return ((u8 *) p)[off];						\
}									\
NI int loc_##N (int i, int off)						\
{									\
  struct W x;								\
  __builtin_memset (&x, 1, sizeof x);					\
  x.ARR[i] = 7;								\
  return ((u8 *) &x)[off];						\
}									\
NI int zi_##N (int i, int off)						\
{									\
  struct W x = {};							\
  u8 t[sizeof x];							\
  x.ARR[i] = 7;								\
  __builtin_memcpy (t, &x, sizeof x);					\
  return t[off];							\
}									\
NI int ext_##N (struct W *p, int i)					\
{ p->ARR[i] = 1; clob (p, 7); return p->ARR[i]; }

T (f, WF, h.d)
T (1, W1, h.d)
T (0, W0, h.d)
T (m, M0, mark)

#define R(N, W, I, OFF)							\
  do {									\
    struct W a, b;							\
    __builtin_memset (&a, 0, sizeof a);					\
    __builtin_memset (&b, 7, sizeof b);					\
    if (raw_##N (&a, I, OFF) != 7					\
	|| raw2_##N (&a, I, OFF) != 7					\
	|| ms_##N (&a, I) != 7						\
	|| ms2_##N (&a, I, OFF) != 7					\
	|| cp_##N (&a, &b, I) != 7					\
	|| cp2_##N (&a, &b, I, OFF) != 7				\
	|| loc_##N (I, OFF) != 7					\
	|| zi_##N (I, OFF) != 7						\
	|| ext_##N (&a, I) != 7)					\
      __builtin_abort ();						\
  } while (0)

int
main (void)
{
  R (f, WF, 3, 4);
  R (1, W1, 3, 4);
  R (0, W0, 3, 4);
  R (m, M0, 2, 3);
  return 0;
}
