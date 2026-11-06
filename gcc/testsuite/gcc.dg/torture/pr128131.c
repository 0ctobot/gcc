/* PR tree-optimization/128131 */
/* { dg-do run } */
/* { dg-require-effective-target strndup } */
/* { dg-skip-if "object sizes need optimization" { *-*-* } { "-O0" } { "" } } */
/* { dg-additional-options "-Wno-stringop-overread" } */

typedef __SIZE_TYPE__ size_t;

extern void abort (void);
extern void free (void *);
extern void *malloc (size_t);
extern void *memset (void *, int, size_t);
extern char *strndup (const char *, size_t);
extern size_t strlen (const char *);

#define NO_LIMIT __SIZE_MAX__
#define UNKNOWN __SIZE_MAX__

char no_nul[4] = "abcd";
char with_nul[4] = "abc";
struct S { char a[4]; char b[4]; } s = { "abcd", "efg" };

/* The duplicate takes strlen (P) + 1 bytes, which the maximum estimate must
   cover and the minimum estimate cannot exceed.  */
#define CHECK_ALLOCATION(p, max, min)					\
  size_t real = strlen (p) + 1;						\
  if (real > (max) || real < (min))					\
    abort ();								\
  if (__builtin_dynamic_object_size (p, 0) != (max)			\
      || __builtin_dynamic_object_size (p, 1) != (max)			\
      || __builtin_dynamic_object_size (p, 2) != (min)			\
      || __builtin_dynamic_object_size (p, 3) != (min))			\
    abort ()

/* The size is known at compile time.  */
#define TEST_STATIC(expr, max, min)					\
  do									\
    {									\
      char *p = (expr);							\
      CHECK_ALLOCATION (p, max, min);					\
      if (__builtin_object_size (p, 0) != (max)				\
	  || __builtin_object_size (p, 1) != (max)			\
	  || __builtin_object_size (p, 2) != (min)			\
	  || __builtin_object_size (p, 3) != (min))			\
	abort ();							\
      free (p);								\
    }									\
  while (0)

/* The size is only known at run time, so a static estimate can be unknown but
   it still cannot be wrong.  */
#define TEST_DYNAMIC(expr, max, min)					\
  do									\
    {									\
      char *p = (expr);							\
      CHECK_ALLOCATION (p, max, min);					\
      if (__builtin_object_size (p, 0) < real				\
	  || __builtin_object_size (p, 1) < real			\
	  || __builtin_object_size (p, 2) > real			\
	  || __builtin_object_size (p, 3) > real)			\
	abort ();							\
      free (p);								\
    }									\
  while (0)

__attribute__ ((noipa)) void
constant_bound (const char *unknown, int c)
{
  /* A bound that fits in the source needs no NUL there and gets one appended,
     while a larger bound needs the NUL in the source.  */
  TEST_STATIC (strndup (no_nul, 4), 5, 1);
  TEST_STATIC (strndup (no_nul, 3), 4, 1);
  TEST_STATIC (strndup (with_nul, 5), 4, 1);
  TEST_STATIC (strndup (with_nul, NO_LIMIT), 4, 1);
  TEST_STATIC (strndup (&no_nul[2], 2), 3, 1);
  TEST_STATIC (strndup (&no_nul[4], 0), 1, 1);
  TEST_STATIC (strndup (c ? no_nul : with_nul, 4), 5, 1);

  /* A member doesn't limit the duplicate of a string that runs past it.  */
  TEST_STATIC (strndup (s.a, 2), 3, 1);
  TEST_STATIC (strndup (s.a, 4), 5, 1);
  TEST_STATIC (strndup (s.a, 8), 9, 1);

  TEST_STATIC (strndup ("hello", 3), 4, 4);
  TEST_STATIC (strndup ("hello", 9), 6, 6);
  TEST_STATIC (strndup ("hello", NO_LIMIT), 6, 6);

  TEST_STATIC (strndup (unknown, 3), 4, 1);
  TEST_STATIC (strndup (unknown, NO_LIMIT - 1), UNKNOWN, 1);
  TEST_STATIC (strndup (unknown, NO_LIMIT), UNKNOWN, 1);
}

__attribute__ ((noipa)) void
runtime_bound (const char *unknown, size_t n, size_t max_with_nul,
	       size_t max_string, size_t min_string, size_t max_unknown)
{
  TEST_DYNAMIC (strndup (no_nul, n < 4 ? n : 4), n < 4 ? n + 1 : 5, 1);
  TEST_DYNAMIC (strndup (with_nul, n), max_with_nul, 1);
  TEST_DYNAMIC (strndup (s.a, n < 8 ? n : 8), n < 8 ? n + 1 : 9, 1);
  TEST_DYNAMIC (strndup ("hello", n), max_string, min_string);
  TEST_DYNAMIC (strndup (unknown, n), max_unknown, 1);
}

__attribute__ ((noipa)) void
runtime_source (size_t m, size_t off)
{
  char *src = malloc (m);

  if (!src)
    abort ();
  memset (src, 'x', m);

  TEST_DYNAMIC (strndup (src, m), m + 1, 1);
  TEST_DYNAMIC (strndup (src, m - 1), m, 1);
  TEST_DYNAMIC (strndup (src + off, m - off), m - off + 1, 1);
  TEST_DYNAMIC (strndup (no_nul + off, 4 - off), 5 - off, 1);
  free (src);
}

int
main (void)
{
  constant_bound ("hello", 1);
  constant_bound ("hello", 0);

  runtime_bound ("hello", 2, 3, 3, 3, 3);
  runtime_bound ("hello", 4, 5, 5, 5, 5);
  runtime_bound ("hello", 5, 4, 6, 6, 6);
  runtime_bound ("hello", NO_LIMIT, 4, 6, 6, UNKNOWN);

  runtime_source (4, 1);
  runtime_source (16, 3);
  return 0;
}
