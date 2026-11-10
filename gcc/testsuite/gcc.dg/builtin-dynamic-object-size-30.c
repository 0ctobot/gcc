/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

/* The size from a signed counted_by counter is computed in the counter's
   own type: a count that doesn't fit in int isn't truncated, and a negative
   count gives 0.  Counts that large are impractical to back with real
   storage, so this checks the sizes that get folded for counts set right
   before each query, negative ones included.
   builtin-dynamic-object-size-27.c runs the same queries at run time with
   small counts.  */

typedef __SIZE_TYPE__ size_t;

void link_error (void);

struct cl { long n; char data[] __attribute__ ((counted_by (n))); };
struct cll { long long n; int data[] __attribute__ ((counted_by (n))); };
struct ci { int n; long data[] __attribute__ ((counted_by (n))); };
struct cs { short n; char data[] __attribute__ ((counted_by (n))); };
struct cu { unsigned long n; char data[] __attribute__ ((counted_by (n))); };
struct cp { long n; char *p __attribute__ ((counted_by (n))); };
struct cpi { long long n; int *p __attribute__ ((counted_by (n))); };

/* A flexible array member's counter can be set on its own, but a pointer
   member must be set together with its counter.  */
#define SET_COUNT(P, N, A) ((P)->n = (N))
#define SET_BOTH(P, N, A) (*(P) = (__typeof (*(P))) { (N), (A) })

/* Set the counter of P to VAL, converted to its type, and check that member
   M has the count times its element size, or 0 for a negative count.  */
#define EQ(SET, P, A, M, T, VAL)					\
  do									\
    {									\
      __typeof ((P)->n) n_ = (__typeof ((P)->n)) (VAL);			\
      SET (P, n_, A);							\
      if (__builtin_dynamic_object_size ((P)->M, T)			\
	  != (n_ < 0 ? 0 : (size_t) n_ * sizeof ((P)->M[0])))		\
	link_error ();							\
    }									\
  while (0)

#define VALS(SET, P, A, M, T)						\
  EQ (SET, P, A, M, T, 0x7fffffffLL);					\
  EQ (SET, P, A, M, T, 0x80000000LL);					\
  EQ (SET, P, A, M, T, 0xffffffffLL);					\
  EQ (SET, P, A, M, T, 0x100000001LL);					\
  EQ (SET, P, A, M, T, -1);						\
  EQ (SET, P, A, M, T, -0x80000000LL);					\
  EQ (SET, P, A, M, T, -0x100000000LL)

void
f_l (struct cl *p)
{
  VALS (SET_COUNT, p, 0, data, 1);
  VALS (SET_COUNT, p, 0, data, 0);
}

void f_ll (struct cll *p) { VALS (SET_COUNT, p, 0, data, 1); }
void f_i (struct ci *p) { VALS (SET_COUNT, p, 0, data, 1); }
void f_s (struct cs *p) { VALS (SET_COUNT, p, 0, data, 1); }
void f_u (struct cu *p) { VALS (SET_COUNT, p, 0, data, 1); }
void f_p (struct cp *p, char *a) { VALS (SET_BOTH, p, a, p, 1); }
void f_pi (struct cpi *p, int *a) { VALS (SET_BOTH, p, a, p, 1); }

/* { dg-final { scan-tree-dump-not "link_error" "optimized" } } */
