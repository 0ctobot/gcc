/* { dg-do run } */

/* A builtin declared noreturn is an ordinary call: every call below must reach
   its callee instead of being folded or expanded as the builtin, which used to
   fall through into the else arm or off the end of the function.  The asm
   labels keep the callees from replacing the C library's functions.  */

#include <setjmp.h>

typedef __SIZE_TYPE__ size_t;

extern void *memset (void *, int, size_t)
  __asm__ ("my_memset") __attribute__((noreturn));
extern void *memcpy (void *, const void *, size_t)
  __asm__ ("my_memcpy") __attribute__((noreturn));
extern void *memmove (void *, const void *, size_t)
  __asm__ ("my_memmove") __attribute__((noreturn));
extern char *strcpy (char *, const char *)
  __asm__ ("my_strcpy") __attribute__((noreturn));
extern char *strcat (char *, const char *)
  __asm__ ("my_strcat") __attribute__((noreturn));
extern int sprintf (char *, const char *, ...)
  __asm__ ("my_sprintf") __attribute__((noreturn));
extern void free (void *)
  __asm__ ("my_free") __attribute__((noreturn));
extern void *malloc (size_t);
extern void *calloc (size_t, size_t);
extern size_t strlen (const char *);
extern void abort (void);

struct s { int a[4]; };

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

__attribute__((used)) void *
my_memmove (void *p, const void *q, size_t n)
{
  longjmp (env, 1);
}

__attribute__((used)) char *
my_strcpy (char *p, const char *q)
{
  longjmp (env, 1);
}

__attribute__((used)) char *
my_strcat (char *p, const char *q)
{
  longjmp (env, 1);
}

__attribute__((used)) int
my_sprintf (char *p, const char *f, ...)
{
  longjmp (env, 1);
}

__attribute__((used)) void
my_free (void *p)
{
  longjmp (env, 1);
}

__attribute__((noipa)) void
h (void)
{
  abort ();
}

/* DSE deleted these as redundant stores.  */

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
t2 (struct s *p)
{
  *p = (struct s) { 0 };
  if (g)
    memset (p, 0, sizeof *p);
  else
    h ();
}

/* The strlen pass deleted this as clearing already zeroed memory.  */

__attribute__((noipa)) char *
t3 (size_t n)
{
  char *p = calloc (n, 1);
  if (g)
    memset (p, 0, n);
  else
    h ();
  return p;
}

/* Folded while gimplifying, after the then arm was judged not to fall through,
   so control ran straight into the else arm.  */

__attribute__((noipa)) void
t4 (char *p)
{
  if (g)
    memset (p, 0, 0);
  else
    h ();
}

__attribute__((noipa)) void
t5 (char *p, const char *q)
{
  if (g)
    memcpy (p, q, 0);
  else
    h ();
}

__attribute__((noipa)) void
t6 (char *p, size_t n)
{
  if (g)
    memmove (p, p, n);
  else
    h ();
}

/* Expanded inline with nothing after the copy.  */

__attribute__((noipa)) void
t7 (char *p, const char *q)
{
  if (g)
    memcpy (p, q, 4);
  else
    h ();
}

/* Replaced by stores or by a call to a builtin that does return.  */

__attribute__((noipa)) void
t8 (char *p)
{
  if (g)
    strcpy (p, "abc");
  else
    h ();
}

__attribute__((noipa)) void
t9 (char *p)
{
  if (g)
    sprintf (p, "abc");
  else
    h ();
}

__attribute__((noipa)) void
t10 (char *p, const char *q)
{
  size_t l = strlen (q);
  if (l > 2 && g)
    strcpy (p, q);
  else
    h ();
}

__attribute__((noipa)) void
t11 (char *p, const char *q)
{
  size_t l = strlen (q);
  size_t m = strlen (p);
  if (l + m > 2 && g)
    strcat (p, q);
  else
    h ();
}

/* DCE deleted the allocation together with the free.  */

__attribute__((noipa)) void
t12 (size_t n)
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
  static char a[64];
  static char b[64] = "abcdefgh";
  static struct s c;

  g = 1;
  RUN (t1 (a));
  RUN (t2 (&c));
  RUN (t3 (8));
  RUN (t4 (a));
  RUN (t5 (a, b));
  RUN (t6 (a, 8));
  RUN (t7 (a, b));
  RUN (t8 (a));
  RUN (t9 (a));
  RUN (t10 (a, b));
  RUN (t11 (a, b));
  RUN (t12 (8));
  return 0;
}
