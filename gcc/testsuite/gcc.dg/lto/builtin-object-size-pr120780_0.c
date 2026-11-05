/* PR middle-end/120780: free_lang_data rewrites a pointer to a record in the
   type of a field to a pointer to the record's incomplete variant, so a type
   match on the field can't tell the size of a pointer member's container after
   LTO.  The object sizes of such a container must not depend on that match.  */
/* { dg-lto-do run } */
/* { dg-lto-options { { -O2 -flto } } } */

#include "../builtin-object-size-common.h"

#define BOS __builtin_object_size
#define DOS __builtin_dynamic_object_size

void *alloc (size_t) __attribute__ ((alloc_size (1), malloc, noinline));

void *
alloc (size_t n)
{
  return calloc (1, n);
}

struct inner { int d[4]; };
struct w1 { int rate[6]; struct inner *m; };
struct w2 { int rate[6]; struct { int d; struct inner *m; } n; };

#define CHECK(W, c, mq)							\
  do {									\
    EXPECT (BOS (c, 0), sizeof (W));					\
    EXPECT (BOS (c, 1), sizeof (W));					\
    EXPECT (BOS ((c)->rate, 0), sizeof (W));				\
    EXPECT (BOS ((c)->rate, 1), sizeof ((c)->rate));			\
    EXPECT (BOS (mq, 1), sizeof (*(mq)));				\
    EXPECT (DOS (c, 0), sizeof (W));					\
    EXPECT (DOS (c, 1), sizeof (W));					\
    EXPECT (DOS ((c)->rate, 0), sizeof (W));				\
    EXPECT (DOS ((c)->rate, 1), sizeof ((c)->rate));			\
    EXPECT (DOS (mq, 1), sizeof (*(mq)));				\
  } while (0)

static inline __attribute__ ((always_inline)) void
w1_early_child (struct inner **p)
{
  struct w1 *c = (struct w1 *) ((char *) p - __builtin_offsetof (struct w1, m));
  CHECK (struct w1, c, &c->m);
}

static void
w1_late_child (struct inner **p)
{
  struct w1 *c = (struct w1 *) ((char *) p - __builtin_offsetof (struct w1, m));
  CHECK (struct w1, c, &c->m);
}

static inline __attribute__ ((always_inline)) void
w2_early_child (struct inner **p)
{
  struct w2 *c = (struct w2 *) ((char *) p
				- __builtin_offsetof (struct w2, n.m));
  CHECK (struct w2, c, &c->n.m);
}

static void
w2_late_child (struct inner **p)
{
  struct w2 *c = (struct w2 *) ((char *) p
				- __builtin_offsetof (struct w2, n.m));
  CHECK (struct w2, c, &c->n.m);
}

int
main (void)
{
  struct w1 *a = alloc (sizeof *a);
  struct w2 *b = alloc (sizeof *b);

  w1_early_child (&a->m);
  w1_late_child (&a->m);
  w2_early_child (&b->n.m);
  w2_late_child (&b->n.m);

  DONE ();
}
