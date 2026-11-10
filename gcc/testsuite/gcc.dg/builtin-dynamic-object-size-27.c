/* { dg-do run } */
/* { dg-options "-O2" } */

/* The size from a counted_by counter is the count times the element size.
   Every count here is backed by real storage, which keeps the counts small:
   builtin-dynamic-object-size-30.c checks counts that don't fit in int and
   negative counts.  */

typedef __SIZE_TYPE__ size_t;

#ifdef PRINT
int printf (const char *, ...);
#define EQ(expr, want) \
  printf ("%-10s %#18zx  want %#zx\n", #expr, (size_t) (expr), (size_t) (want))
#else
#define EQ(expr, want) if ((expr) != (want)) __builtin_abort ()
#endif

void *malloc (size_t);

struct cl { long n; char data[] __attribute__ ((counted_by (n))); };
struct cll { long long n; int data[] __attribute__ ((counted_by (n))); };
struct ci { int n; long data[] __attribute__ ((counted_by (n))); };
struct cs { short n; char data[] __attribute__ ((counted_by (n))); };
struct cu { unsigned long n; char data[] __attribute__ ((counted_by (n))); };
struct cp { long n; char *p __attribute__ ((counted_by (n))); };
struct cpi { long n; int *p __attribute__ ((counted_by (n))); };

__attribute__ ((noipa)) size_t
sz_l (struct cl *p)
{
  return __builtin_dynamic_object_size (p->data, 1);
}

__attribute__ ((noipa)) size_t
sz_l0 (struct cl *p)
{
  return __builtin_dynamic_object_size (p->data, 0);
}

__attribute__ ((noipa)) size_t
sz_ll (struct cll *p)
{
  return __builtin_dynamic_object_size (p->data, 1);
}

__attribute__ ((noipa)) size_t
sz_i (struct ci *p)
{
  return __builtin_dynamic_object_size (p->data, 1);
}

__attribute__ ((noipa)) size_t
sz_s (struct cs *p)
{
  return __builtin_dynamic_object_size (p->data, 1);
}

__attribute__ ((noipa)) size_t
sz_u (struct cu *p)
{
  return __builtin_dynamic_object_size (p->data, 1);
}

/* A pointer member gets its size from its counter the same way.  */

__attribute__ ((noipa)) size_t
sz_p (struct cp *p)
{
  return __builtin_dynamic_object_size (p->p, 1);
}

__attribute__ ((noipa)) size_t
sz_pi (struct cpi *p)
{
  return __builtin_dynamic_object_size (p->p, 1);
}

int
main (void)
{
  /* Allocate the elements for each count and set a pointer member together
     with its counter, as counted_by requires.  */
  for (int n = 0; n <= 10; n += 5)
    {
      struct cl *l = malloc (sizeof *l + n * sizeof (char));
      struct cll *ll = malloc (sizeof *ll + n * sizeof (int));
      struct ci *i = malloc (sizeof *i + n * sizeof (long));
      struct cs *s = malloc (sizeof *s + n * sizeof (char));
      struct cu *u = malloc (sizeof *u + n * sizeof (char));
      struct cp *cp = malloc (sizeof *cp);
      struct cpi *cpi = malloc (sizeof *cpi);

      l->n = ll->n = i->n = s->n = u->n = n;
      *cp = (struct cp) { n, malloc (n * sizeof (char)) };
      *cpi = (struct cpi) { n, malloc (n * sizeof (int)) };
      EQ (sz_l (l), n);
      EQ (sz_l0 (l), n);
      EQ (sz_ll (ll), n * sizeof (int));
      EQ (sz_i (i), n * sizeof (long));
      EQ (sz_s (s), n);
      EQ (sz_u (u), n);
      EQ (sz_p (cp), n);
      EQ (sz_pi (cpi), n * sizeof (int));
    }
  return 0;
}
