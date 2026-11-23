/* { dg-do run } */
/* { dg-options "-O2" } */

/* A pointer stepped through tag by a loop is passed to a helper that uses
   container_of to get back to the struct and then picks either that pointer
   or an address in body.  After inlining, the address in body is the loop's
   pointer plus a folded constant offset.  Whichever argument of the PHI node
   the object size pass walks first, the address in body must not be sized
   by what's left of tag.  */

typedef __SIZE_TYPE__ size_t;

#define INLINE static inline __attribute__ ((always_inline))
#define NOIPA __attribute__ ((noipa))
#define container_of(p, T, f) ((T *) ((char *) (p) - __builtin_offsetof (T, f)))

struct rec
{
  char tag[2][4];
  char body[32];
};

struct rec g;
volatile int sink;
static int fails;

/* The stores keep both arms of the branch in place, so the order of the PHI
   arguments follows the order of the arms.  */
#define PICK(NAME, S, THEN, ELSE)					\
  INLINE size_t								\
  NAME (char *p, int c)							\
  {									\
    struct rec *r = container_of (p, struct rec, tag[1]);		\
    char *b = &r->body[8];						\
    char *q;								\
    if (c)								\
      {									\
	sink = 1;							\
	q = THEN;							\
      }									\
    else								\
      {									\
	sink = 2;							\
	q = ELSE;							\
      }									\
    return S (q, 1);							\
  }

PICK (s_pick_then, __builtin_object_size, p, b - 4)
PICK (s_pick_else, __builtin_object_size, b - 4, p)
PICK (d_pick_then, __builtin_dynamic_object_size, p, b - 4)
PICK (d_pick_else, __builtin_dynamic_object_size, b - 4, p)

NOIPA size_t
s_then (long n, int c)
{
  char *t = (char *) &g.tag[0];
  for (long i = 0; i < n; i++)
    t += 4;
  return s_pick_then (t, c);
}

NOIPA size_t
s_else (long n, int c)
{
  char *t = (char *) &g.tag[0];
  for (long i = 0; i < n; i++)
    t += 4;
  return s_pick_else (t, c);
}

/* With the loop inlined as well, the static estimate is the cap for
   __builtin_dynamic_object_size.  */

INLINE char *
advance (char *t, long n)
{
  for (long i = 0; i < n; i++)
    t += 4;
  return t;
}

NOIPA size_t
d_then (long n, int c)
{
  return d_pick_then (advance ((char *) &g.tag[0], n), c);
}

NOIPA size_t
d_else (long n, int c)
{
  return d_pick_else (advance ((char *) &g.tag[0], n), c);
}

/* A maximum may be larger than the truth, but never smaller.  */
#define CHECK(expr, truth)						\
  do									\
    {									\
      size_t got = (expr);						\
      if (got < (truth))						\
	{								\
	  __builtin_printf ("%s: %lu, want at least %lu\n", #expr,	\
			    (unsigned long) got,			\
			    (unsigned long) (truth));			\
	  fails++;							\
	}								\
    }									\
  while (0)

int
main (void)
{
  /* With n == 1 the pointer is the address of tag[1], which has 4 bytes left
     in tag, and body[4] has 28 bytes left in body.  */
  CHECK (s_then (1, 1), 4);
  CHECK (s_then (1, 0), 28);
  CHECK (s_else (1, 0), 4);
  CHECK (s_else (1, 1), 28);
  CHECK (d_then (1, 1), 4);
  CHECK (d_then (1, 0), 28);
  CHECK (d_else (1, 0), 4);
  CHECK (d_else (1, 1), 28);

  if (fails)
    __builtin_abort ();
  return 0;
}
