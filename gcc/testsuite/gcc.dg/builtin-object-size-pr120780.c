/* PR middle-end/120780: a negative offset that takes a pointer below its
   subobject, or below the part of its object that's known, must not leave a
   maximum object size smaller than the true size.  A minimum object size must
   not end up larger than the true size either.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

#include "builtin-object-size-common.h"

#define BOS __builtin_object_size
#define DOS __builtin_dynamic_object_size

#define AT_LEAST(p, want)						\
  do {									\
    size_t v_ = (p);							\
    if (v_ < (size_t) (want))						\
      {									\
	__builtin_printf ("%s == %zd, want at least %zd\n", #p, v_,	\
			  (size_t) (want));				\
	FAIL ();							\
      }									\
  } while (0)

#define AT_MOST(p, want)						\
  do {									\
    size_t v_ = (p);							\
    if (v_ > (size_t) (want))						\
      {									\
	__builtin_printf ("%s == %zd, want at most %zd\n", #p, v_,	\
			  (size_t) (want));				\
	FAIL ();							\
      }									\
  } while (0)

volatile int vzero;

void *alloc (size_t) __attribute__ ((alloc_size (1), malloc, noinline));

void *
alloc (size_t n)
{
  return calloc (1, n);
}

struct inner { int d[4]; };
struct w { int rate[6]; struct inner m[3]; };

/* container_of on an element in the middle of an array member.  */

#define CHECK_MID(c)							\
  do {									\
    EXPECT (BOS (c, 0), sizeof (struct w));				\
    EXPECT (BOS (c, 1), sizeof (struct w));				\
    AT_MOST (BOS (c, 2), sizeof (struct w));				\
    AT_MOST (BOS (c, 3), sizeof (struct w));				\
    EXPECT (BOS ((c)->rate, 0), sizeof (struct w));			\
    EXPECT (BOS ((c)->rate, 1), sizeof ((c)->rate));			\
    AT_MOST (BOS ((c)->rate, 2), sizeof (struct w));			\
    AT_MOST (BOS ((c)->rate, 3), sizeof ((c)->rate));			\
    EXPECT (BOS (&(c)->m[1], 1), 2 * sizeof (struct inner));		\
    AT_MOST (BOS (&(c)->m[1], 3), 2 * sizeof (struct inner));		\
    EXPECT (DOS (c, 0), sizeof (struct w));				\
    EXPECT (DOS (c, 1), sizeof (struct w));				\
    AT_MOST (DOS (c, 2), sizeof (struct w));				\
    AT_MOST (DOS (c, 3), sizeof (struct w));				\
    EXPECT (DOS ((c)->rate, 0), sizeof (struct w));			\
    EXPECT (DOS ((c)->rate, 1), sizeof ((c)->rate));			\
    AT_MOST (DOS ((c)->rate, 2), sizeof (struct w));			\
    AT_MOST (DOS ((c)->rate, 3), sizeof ((c)->rate));			\
    EXPECT (DOS (&(c)->m[1], 1), 2 * sizeof (struct inner));		\
    AT_MOST (DOS (&(c)->m[1], 3), 2 * sizeof (struct inner));		\
  } while (0)

static inline __attribute__ ((always_inline)) void
mid_early_child (struct inner *p)
{
  struct w *c = (struct w *) ((char *) p - __builtin_offsetof (struct w, m[1]));
  CHECK_MID (c);
}

/* This one is big enough to be inlined only after the early passes.  */
static void
mid_late_child (struct inner *p)
{
  struct w *c = (struct w *) ((char *) p - __builtin_offsetof (struct w, m[1]));
  CHECK_MID (c);
}

static void __attribute__ ((noinline))
mid_ipa_child (struct inner *p)
{
  struct w *c = (struct w *) ((char *) p - __builtin_offsetof (struct w, m[1]));
  CHECK_MID (c);
}

static struct w gw;

void __attribute__ ((noinline))
test_mid (void)
{
  struct w sv;
  struct w *s = alloc (sizeof *s);

  mid_early_child (&sv.m[1]);
  __asm__ ("" : : "r" (&sv) : "memory");
  mid_early_child (&s->m[1]);
  mid_late_child (&s->m[1]);
  mid_ipa_child (&gw.m[1]);
}

/* A merge with a separate guard around the negative offset, where the largest
   arm is at the start of its object.  The guard only lets the other arm
   through, but the maximum of the sizes and the maximum of the whole sizes
   still come from different arms.  */

struct wr { long hdr; int rate[6]; };

void __attribute__ ((noinline))
test_guarded (int c, int g)
{
  char *a = alloc (64);
  char *b = alloc (32);
  char *p = c ? a : b + 8;

  if (g)
    {
      struct wr *r = (struct wr *) (p - __builtin_offsetof (struct wr, rate));

      AT_LEAST (BOS (r->rate, 0), 24);
      AT_LEAST (BOS (r->rate, 1), 24);
      AT_MOST (BOS (r->rate, 2), 24);
      AT_MOST (BOS (r->rate, 3), 24);
      EXPECT (DOS (r->rate, 0), 24);
      EXPECT (DOS (r->rate, 1), 24);
      AT_MOST (DOS (r->rate, 2), 24);
      AT_MOST (DOS (r->rate, 3), 24);
    }
}

/* A merge of a pointer that stays in a subobject and one that leaves it.  */

struct big { long pad[8]; char a[64]; char tail[8]; };
struct wt { int rate[6]; struct inner m; long tail[4]; };

void __attribute__ ((noinline))
test_merged (int c)
{
  struct big *x = alloc (sizeof *x);
  struct wt *y = alloc (sizeof *y);
  char *p = c ? &x->a[56] : (char *) &y->m;
  char *q = p - 24;

  AT_LEAST (BOS (q, 0), c ? 40 : 72);
  AT_LEAST (BOS (q, 1), c ? 32 : 72);
  EXPECT (DOS (q, 0), c ? 40 : 72);
  EXPECT (DOS (q, 1), c ? 32 : 72);
}

/* Staying in a subobject keeps the precision of the subobject.  A pointer that
   left it isn't tied to it anymore, even if it comes back later.  */

struct ws { long pad; char a[16]; long tail[4]; };

void __attribute__ ((noinline))
test_constant_offsets (void)
{
  struct ws *s = alloc (sizeof *s);
  char *p = &s->a[8];
  char *stay = p - 4;
  char *leave = p - 16;
  char *back = leave + 8;

  EXPECT (BOS (stay, 0), 44);
  EXPECT (BOS (stay, 1), 12);
  EXPECT (DOS (stay, 1), 12);
  EXPECT (BOS (leave, 0), 56);
  EXPECT (BOS (leave, 1), 56);
  EXPECT (DOS (leave, 1), 56);
  AT_LEAST (BOS (back, 1), 16);
  AT_LEAST (DOS (back, 1), 16);
}

/* A size only known at run time.  */

static void
runtime_child (struct inner *p, size_t n)
{
  struct w *c = (struct w *) ((char *) p - __builtin_offsetof (struct w, m[1]));
  char *q = (char *) p - 8;

  EXPECT (DOS (c, 0), n);
  EXPECT (DOS (c, 1), n);
  EXPECT (DOS (c->rate, 0), n);
  EXPECT (DOS (c->rate, 1), sizeof (c->rate));
  AT_MOST (DOS (c->rate, 3), sizeof (c->rate));
  EXPECT (DOS (q, 1), n - 32);
  AT_MOST (DOS (q, 3), n - 32);
}

void __attribute__ ((noinline))
test_runtime (size_t n)
{
  struct w *s = alloc (n);

  runtime_child (&s->m[1], n);
}

/* An offset only known at run time that may leave the subobject downward.  */

void __attribute__ ((noinline))
test_runoff (size_t n, long i)
{
  struct ws *s = alloc (n);
  char *q = &s->a[8] + i;

  EXPECT (DOS (q, 0), n - 16 - i);
  EXPECT (DOS (q, 1), i >= -8 ? 8 - i : n - 16 - i);
}

void __attribute__ ((noinline))
test_runoff0 (size_t n, long i)
{
  struct ws *s = alloc (n);
  char *q = &s->a[0] + i;

  EXPECT (DOS (q, 0), n - 8 - i);
  EXPECT (DOS (q, 1), i >= 0 ? 16 - i : n - 8 - i);
}

static inline __attribute__ ((always_inline)) void
runback_child (char *p, size_t n, long back)
{
  char *q = p - back;

  EXPECT (DOS (q, 0), n - 16 + back);
  EXPECT (DOS (q, 1), back > 8 ? n - 16 + back : 8 + back);
}

void __attribute__ ((noinline))
test_runback (size_t n, long back)
{
  struct ws *s = alloc (n);

  runback_child (&s->a[8], n, back);
}

/* A size from counted_by only describes the tail of the pointer, so going below
   the pointer's start isn't an underflow.  */

struct cb { int cnt; long hdr; char fam[] __attribute__ ((counted_by (cnt))); };

void __attribute__ ((noinline))
test_counted_by (int cnt)
{
  struct cb *s = alloc (sizeof (struct cb) + cnt);
  char *f, *q;

  s->cnt = cnt;
  f = s->fam;
  q = f - 8;
  EXPECT (DOS (s->fam, 0), cnt);
  EXPECT (DOS (s->fam, 1), cnt);
  EXPECT (DOS (q, 0), cnt + 8);
  EXPECT (DOS (q, 1), cnt + 8);
}

/* The same for a size from an access attribute on a parameter.  */

struct wp { int rate[6]; struct inner m[2]; };
static struct wp gwp;

void __attribute__ ((noinline, access (read_write, 1, 2)))
test_access_parm (struct inner *p, int n)
{
  struct wp *c = (struct wp *) ((char *) p - __builtin_offsetof (struct wp, m));
  char *q = (char *) p - 8;

  EXPECT (DOS (c, 0), sizeof (*c));
  EXPECT (DOS (c, 1), sizeof (*c));
  EXPECT (DOS (c->rate, 0), sizeof (*c));
  EXPECT (DOS (c->rate, 1), sizeof (c->rate));
  EXPECT (DOS (q, 0), 40);
  EXPECT (DOS (q, 1), 40);
}

/* A pointer that's really below the start of its object has no size left.  The
   static maximum isn't checked here since any value is an upper bound of 0.  */

void __attribute__ ((noinline))
test_underflow (long i)
{
  char *a = alloc (16);
  struct ws *s = alloc (sizeof *s);
  char *below1 = a - 8;
  char *below2 = &s->a[0] - 16;
  char *below3 = a + i;

  EXPECT (BOS (below1, 2), 0);
  EXPECT (BOS (below1, 3), 0);
  EXPECT (DOS (below1, 0), 0);
  EXPECT (DOS (below1, 1), 0);
  EXPECT (BOS (below2, 2), 0);
  EXPECT (BOS (below2, 3), 0);
  EXPECT (DOS (below2, 0), 0);
  EXPECT (DOS (below2, 1), 0);
  EXPECT (DOS (below3, 0), 0);
  EXPECT (DOS (below3, 1), 0);
}

int
main (void)
{
  test_mid ();
  test_guarded (vzero, vzero + 1);
  test_guarded (vzero + 1, vzero);
  test_merged (vzero);
  test_merged (vzero + 1);
  test_constant_offsets ();
  test_runtime (72 + vzero);
  test_runtime (200 + vzero);
  test_runoff (56 + vzero, vzero);
  test_runoff (56 + vzero, vzero - 4);
  test_runoff (56 + vzero, vzero - 16);
  test_runoff0 (56 + vzero, vzero);
  test_runoff0 (56 + vzero, vzero + 4);
  test_runoff0 (56 + vzero, vzero - 8);
  test_runback (56 + vzero, vzero);
  test_runback (56 + vzero, vzero + 4);
  test_runback (56 + vzero, vzero + 16);
  test_counted_by (20 + vzero);
  test_access_parm (&gwp.m[0], 2 + vzero);
  test_underflow (vzero - 8);

  DONE ();
}
