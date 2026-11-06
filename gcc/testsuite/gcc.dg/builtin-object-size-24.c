/* Folding can turn container_of, a member access and a pointer step into a
   single positive offset from the member that container_of started at.  A
   maximum subobject size must not take that for a pointer leaving the member,
   since it may have landed in another subobject with room left.  A constant
   offset straight from the source that leaves its subobject must still give
   0.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

#include "builtin-object-size-common.h"

#define BOS __builtin_object_size
#define DOS __builtin_dynamic_object_size
#define INLINE static inline __attribute__ ((always_inline))
#define NOIPA __attribute__ ((noipa))
#define container_of(p, T, f) ((T *) ((char *) (p) - __builtin_offsetof (T, f)))

#define CHECK(got, want)						\
  do									\
    {									\
      if ((got) != (want))						\
	FAIL ();							\
    }									\
  while (0)

/* A maximum may be larger than the truth, but never smaller.  */
#define CHECK_MAX(got, truth, limit)					\
  do									\
    {									\
      if ((got) < (truth) || (got) > (limit))				\
	FAIL ();							\
    }									\
  while (0)

struct inner
{
  char b[16];
};

struct w
{
  char h[40];
  struct inner m[3];
  char pad[8];
  char a[16];
  char tail[8];
};

#define CHILDREN(NAME, S)						\
  INLINE size_t								\
  NAME##_stay (char *p)							\
  {									\
    struct w *s = container_of (p, struct w, m[1]);			\
    char *a = &s->a[8];							\
    char *q = a - 4;							\
    return S (q, 1);							\
  }									\
  INLINE size_t								\
  NAME##_steps (char *p)						\
  {									\
    struct w *s = container_of (p, struct w, m[1]);			\
    char *q = &s->a[0];							\
    q = q + 4;								\
    q = q + 4;								\
    q = q - 2;								\
    return S (q, 1);							\
  }									\
  INLINE size_t								\
  NAME##_next (char *p)							\
  {									\
    struct w *s = container_of (p, struct w, m[1]);			\
    char *q = &s->pad[2];						\
    q = q + 1;								\
    return S (q, 1);							\
  }									\
  INLINE size_t								\
  NAME##_runtime (char *p, long i)					\
  {									\
    struct w *s = container_of (p, struct w, m[1]);			\
    char *a = s->a;							\
    char *q = a + 4;							\
    q += i;								\
    return S (q, 1);							\
  }

CHILDREN (s, BOS)
CHILDREN (d, DOS)

#define CHECK_CHILDREN(p)						\
  do									\
    {									\
      CHECK (s_stay (p), 12);						\
      CHECK (d_stay (p), 12);						\
      CHECK (s_steps (p), 10);						\
      CHECK (d_steps (p), 10);						\
      CHECK (s_next (p), 5);						\
      CHECK (d_next (p), 5);						\
    }									\
  while (0)

struct w g;

NOIPA void
test_param (struct w *x, long one)
{
  CHECK_CHILDREN ((char *) &x->m[1]);
  CHECK_CHILDREN ((char *) &x->m[one]);
  CHECK_MAX (d_runtime ((char *) &x->m[1], one + 1), 10, (size_t) -1);
}

NOIPA void
test_global (long one)
{
  CHECK_CHILDREN ((char *) &g.m[1]);
  CHECK_CHILDREN ((char *) &g.m[one]);
}

NOIPA void
test_heap (void)
{
  struct w *x = __builtin_malloc (sizeof *x);
  CHECK_CHILDREN ((char *) &x->m[1]);
  __builtin_free (x);
}

/* The step lands in the member after a loop picked the pointer.  */

NOIPA void
test_loop (struct w *x, long n)
{
  char *t = (char *) &x->m[0];
  for (long i = 0; i < n; i++)
    t += 16;
  CHECK_MAX (s_stay (t), 12, (size_t) -1);
  CHECK (d_stay (t), 12);
}

/* Steps that stay in the member for both of the pointers merged by a PHI don't
   tell anything about the enclosing object.  */

NOIPA void
test_phi (int c)
{
  char *t = c ? (char *) &g.m[2] : (char *) &g.m[1];
  char *u = c ? t - 8 : t - 16;
  CHECK_MAX (BOS (u, 1), c ? 24 : 48, 48);
  CHECK_MAX (DOS (u, 1), c ? 24 : 48, 48);
}

/* The statements in a function of its own aren't folded when the early pass
   looks at them, so these really do leave the subobject.  */

NOIPA void
test_exit (struct w *x, long i)
{
  char *p = x->a;
  CHECK (BOS (p + 4, 1), 12);
  CHECK (DOS (p + 4, 1), 12);
  CHECK (BOS (p + 16, 1), 0);
  CHECK (DOS (p + 16, 1), 0);
  CHECK (BOS (p + 20, 1), 0);
  CHECK (DOS (p + 20, 1), 0);
  /* A non-constant offset cannot be told from a folded one, and the object
     around the member is unknown here, so the size is at least one that no
     object can have.  */
  CHECK_MAX (DOS (p + i, 1), (size_t) -1 / 2, (size_t) -1);
}

int
main (void)
{
  struct w *x = __builtin_malloc (sizeof *x);
  test_param (x, 1);
  test_global (1);
  test_heap ();
  test_loop (x, 1);
  test_phi (0);
  test_phi (1);
  test_exit (x, 20);
  __builtin_free (x);
  DONE ();
}
