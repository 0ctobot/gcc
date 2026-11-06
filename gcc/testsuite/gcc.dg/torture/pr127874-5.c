/* PR tree-optimization/127874 */
/* { dg-do run } */
/* { dg-prune-output "function called through a non-compatible type" } */

/* Same as pr127874-4.c, but the noreturn comes from the function pointer type
   the builtin gets called through.  The asm labels keep the callees from
   replacing the C library's functions.  */

#include <setjmp.h>

typedef __SIZE_TYPE__ size_t;

extern void *memset (void *, int, size_t) __asm__ ("my_memset");
extern void *memcpy (void *, const void *, size_t) __asm__ ("my_memcpy");
extern void abort (void);

typedef void *(*nr_memset_t) (void *, int, size_t) __attribute__((noreturn));
typedef void *(*nr_memcpy_t) (void *, const void *, size_t)
  __attribute__((noreturn));

static jmp_buf env;
int g;

__attribute__((used)) void *
my_memset (void *p, int c, size_t n)
{
  longjmp (env, 1);
}

__attribute__((used)) void *
my_memcpy (void *p, const void *q, size_t n)
{
  longjmp (env, 1);
}

__attribute__((noipa)) void
h (void)
{
  abort ();
}

__attribute__((noipa)) void
t1 (char *p, const char *q)
{
  if (g)
    ((nr_memcpy_t) memcpy) (p, q, 4);
  else
    h ();
}

__attribute__((noipa)) void
t2 (char *p)
{
  *p = 0;
  if (g)
    ((nr_memset_t) __builtin_memset) (p, 0, 1);
  else
    h ();
}

__attribute__((noipa)) void
t3 (char *p)
{
  nr_memset_t fp = (nr_memset_t) memset;
  *p = 0;
  if (g)
    fp (p, 0, 1);
  else
    h ();
}

static nr_memset_t sfp = (nr_memset_t) memset;

__attribute__((noipa)) void
t4 (char *p)
{
  *p = 0;
  if (g)
    sfp (p, 0, 1);
  else
    h ();
}

static inline void
call (nr_memset_t fp, char *p)
{
  fp (p, 0, 1);
}

__attribute__((noipa)) void
t5 (char *p)
{
  *p = 0;
  if (g)
    call ((nr_memset_t) memset, p);
  else
    h ();
}

#define RUN(call)		\
  do				\
    {				\
      if (!setjmp (env))	\
	{			\
	  call;			\
	  abort ();		\
	}			\
    }				\
  while (0)

int
main (void)
{
  static char a[64];
  static char b[64] = "abcdefgh";

  g = 1;
  RUN (t1 (a, b));
  RUN (t2 (a));
  RUN (t3 (a));
  RUN (t4 (a));
  RUN (t5 (a));
  return 0;
}
