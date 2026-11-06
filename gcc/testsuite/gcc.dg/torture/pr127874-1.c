/* PR tree-optimization/127874 */
/* { dg-do run } */

/* These definitions of builtins carry no attribute: IPA finds that they never
   return.  The asm labels keep them from replacing the C library's functions.
   The calls must still reach them.  */

#include <setjmp.h>

typedef __SIZE_TYPE__ size_t;

extern void *memset (void *, int, size_t) __asm__ ("my_memset");
extern void free (void *) __asm__ ("my_free");
extern void *malloc (size_t);
extern void abort (void);

static jmp_buf env;
int g;

__attribute__((noinline)) void *
memset (void *p, int c, size_t n)
{
  longjmp (env, 1);
}

__attribute__((noinline)) void
free (void *p)
{
  longjmp (env, 1);
}

__attribute__((noipa)) void
h (void)
{
  abort ();
}

__attribute__((noipa)) void
t1 (char *p)
{
  *p = 0;
  if (g)
    memset (p, 0, 1);
  else
    h ();
}

__attribute__((noipa)) void
t2 (size_t n)
{
  void *p = malloc (n);
  if (g)
    free (p);
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
  static char a[8];

  g = 1;
  RUN (t1 (a));
  RUN (t2 (8));
  return 0;
}
