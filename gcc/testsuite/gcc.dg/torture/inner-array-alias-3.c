/* A variable index into a [1] array that isn't the last member of its
   structure isn't limited to index 0.  A load of e[i] must not be replaced
   by a load of e[0], and a loop over e[i] must not be cut to one
   iteration.  */
/* { dg-do run } */
/* { dg-options "-std=gnu11 -fno-strict-aliasing" } */

#define NI __attribute__((noipa))

struct hdr { int n; int e[1]; };
struct box { struct hdr h; int more[15]; };
struct flat { int n; int e[1]; int more[15]; };

#define OPS(name, type, N, E)						\
NI int sum_##name (type *p)						\
{ int s = 0; for (int i = 0; i < p->N; i++) s += p->E[i]; return s; }	\
NI int find_##name (type *p, int key)					\
{									\
  for (int i = 0; i < p->N; i++)					\
    if (p->E[i] == key)							\
      return i;								\
  return -1;								\
}									\
NI void fill_##name (type *p, int n)					\
{ for (int i = 0; i < n; i++) p->E[i] = i + 1; }			\
NI int same_##name (type *p, int i)					\
{ return p->E[0] == p->E[i]; }						\
NI int count_##name (type *p)						\
{ int i = 0; while (p->E[i]) i++; return i; }				\
NI void check_##name (type *p, int *n, int *e)				\
{									\
  __builtin_memset (p, 0, sizeof *p);					\
  fill_##name (p, 8);							\
  for (int i = 0; i < 8; i++)						\
    if (e[i] != i + 1)							\
      __builtin_abort ();						\
  *n = 8;								\
  if (sum_##name (p) != 36 || find_##name (p, 5) != 4			\
      || same_##name (p, 3) || count_##name (p) != 8)			\
    __builtin_abort ();							\
}

OPS (box, struct box, h.n, h.e)
OPS (flat, struct flat, n, e)

int
main (void)
{
  struct box b;
  struct flat f;

  check_box (&b, &b.h.n, (int *) &b + 1);
  check_flat (&f, &f.n, (int *) &f + 1);
  return 0;
}
