/* Check the object size of strndup when its source depends on its own result
   through a PHI in a loop, which used to make compute_builtin_object_size
   recurse until cc1 ran out of stack.

   A static size has to hold for every number of iterations.  BUF has 8 bytes,
   so 8 is the maximum for a pointer that can still be BUF.  A strndup result
   has at most BOUND + 1 bytes, and a BOUND past the end of the source means the
   NUL is in the source, so the result isn't bigger than the source either.  The
   minimum for a strndup result is 1 byte since the string can be empty.  A
   dynamic size follows the iterations instead, starting from 8 for BUF.

   -fchecking=2 catches a placeholder for a size in the loop that ends up with
   no definition, which a compiler with release checking otherwise accepts.  */
/* { dg-do run } */
/* { dg-options "-O2 -Wno-stringop-overread -fchecking=2" } */
/* { dg-additional-options "-DSKIP_STRNDUP" { target { ! strndup } } } */

#include "builtin-object-size-common.h"

#ifndef SKIP_STRNDUP

#ifdef __builtin_object_size
# define SIZE(static_size, dynamic_size) (dynamic_size)
#else
# define SIZE(static_size, dynamic_size) (static_size)
#endif

char buf[8] = "abc";

#define SELF_LOOP(mode, bound)						\
  __attribute__ ((noipa)) size_t					\
  self_loop_##mode##_##bound (int n)					\
  {									\
    char *p = buf;							\
    for (int i = 0; i < n; i++)						\
      p = strndup (p, bound);						\
    return __builtin_object_size (p, mode);				\
  }

SELF_LOOP (0, 4)
SELF_LOOP (1, 4)
SELF_LOOP (2, 4)
SELF_LOOP (3, 4)
SELF_LOOP (0, 16)
SELF_LOOP (1, 16)
SELF_LOOP (2, 16)
SELF_LOOP (3, 16)

/* The walk starts at the call rather than at the PHI.  */
#define RESULT_LOOP(mode)						\
  __attribute__ ((noipa)) size_t					\
  result_loop_##mode (int n)						\
  {									\
    char *p = buf, *r;							\
    do									\
      {									\
	r = strndup (p, 4);						\
	p = r;								\
      }									\
    while (--n > 0);							\
    return __builtin_object_size (r, mode);				\
  }

RESULT_LOOP (0)
RESULT_LOOP (1)
RESULT_LOOP (2)
RESULT_LOOP (3)

#define OFFSET_LOOP(mode)						\
  __attribute__ ((noipa)) size_t					\
  offset_loop_##mode (int n)						\
  {									\
    char *p = buf;							\
    for (int i = 0; i < n; i++)						\
      p = strndup (p + 1, 4);						\
    return __builtin_object_size (p, mode);				\
  }

OFFSET_LOOP (0)
OFFSET_LOOP (1)

/* Each result starts one byte before the next source, which makes that source
   one byte smaller than the result it comes from.  */
#define NEGATIVE_OFFSET_LOOP(mode)					\
  __attribute__ ((noipa)) size_t					\
  negative_offset_loop_##mode (int n)					\
  {									\
    char *p = buf + 1;							\
    for (int i = 0; i < n; i++)						\
      p = strndup (p, 8) + 1;						\
    return __builtin_object_size (p - 1, mode);				\
  }

NEGATIVE_OFFSET_LOOP (0)
NEGATIVE_OFFSET_LOOP (1)

/* The source is a member of the previous result, which reaches the loop through
   addr_object_size instead of a PLUS or a PHI.  */
struct s { char a[4]; char b[4]; } members = { "abc", "abc" };

#define MEMBER_LOOP(mode)						\
  __attribute__ ((noipa)) size_t					\
  member_loop_##mode (int n)						\
  {									\
    struct s *p = &members;						\
    for (int i = 0; i < n; i++)						\
      p = (struct s *) strndup (p->b, 8);				\
    return __builtin_object_size (p, mode);				\
  }

MEMBER_LOOP (0)
MEMBER_LOOP (1)

#define MEMBER_QUERY_LOOP(mode)						\
  __attribute__ ((noipa)) size_t					\
  member_query_loop_##mode (int n)					\
  {									\
    struct s *p = &members;						\
    char *q = p->b;							\
    for (int i = 0; i < n; i++)						\
      {									\
	p = (struct s *) strndup (q, 8);				\
	q = p->b;							\
      }									\
    return __builtin_object_size (q, mode);				\
  }

MEMBER_QUERY_LOOP (0)
MEMBER_QUERY_LOOP (1)

__attribute__ ((noipa)) size_t
two_calls_loop (int n)
{
  char *p = buf;
  for (int i = 0; i < n; i++)
    {
      char *q = strndup (p, 16);
      p = strndup (q, 4);
    }
  return __builtin_object_size (p, 0);
}

__attribute__ ((noipa)) size_t
diamond_loop (int n)
{
  char *p = buf;
  for (int i = 0; i < n; i++)
    if (i & 1)
      p = strndup (p, 4);
  return __builtin_object_size (p, 0);
}

/* memcpy returns its first argument, so the cycle goes through a second call.
   The variable length keeps the call around.  */
__attribute__ ((noipa)) size_t
pass_through_loop (int n)
{
  char *p = buf;
  for (int i = 0; i < n; i++)
    p = memcpy (strndup (p, 4), buf, n & 1);
  return __builtin_object_size (p, 0);
}

__attribute__ ((noipa)) size_t
query_in_loop (int n)
{
  char *p = buf;
  size_t last = 0;
  for (int i = 0; i < n; i++)
    {
      last = __builtin_object_size (p, 0);
      p = strndup (p, 4);
    }
  return last;
}

/* A variable bound leaves no constant for a static size to use.  */
__attribute__ ((noipa)) size_t
variable_bound_loop (int n, size_t bound)
{
  char *p = buf;
  for (int i = 0; i < n; i++)
    p = strndup (p, bound);
  return __builtin_object_size (p, 0);
}

/* The incoming size has no static value but it does have a dynamic one.  */
__attribute__ ((noipa)) size_t
variable_source_loop (int n, size_t size)
{
  char *p = malloc (size);
  strcpy (p, "abc");
  for (int i = 0; i < n; i++)
    p = strndup (p, 4);
  return __builtin_object_size (p, 0);
}

/* The size of S isn't known in either mode, which has to turn the placeholder
   for the loop's PHI into an unknown size without leaving any use of it behind
   in the size of R.  */
__attribute__ ((noipa)) size_t
unknown_source_loop (int n, char *s, size_t *rsize)
{
  char *p = s, *r = s;
  for (int i = 0; i < n; i++)
    {
      r = strndup (p, 4);
      p = r;
    }
  *rsize = __builtin_object_size (r, 0);
  return __builtin_object_size (p, 0);
}

/* The PHI only finds out that S has no known size after the walk went through
   the call, which by then holds a placeholder for the size of that PHI.  An
   unknown size leaves that placeholder with no definition, so the size of the
   call can't keep using it.  */
#define LATE_UNKNOWN_LOOP(mode, name, s_iter)				\
  __attribute__ ((noipa)) size_t					\
  late_unknown_##name##_##mode (int n, char *s, size_t *rsize)		\
  {									\
    char *p = buf, *r = buf;						\
    for (int i = 0; i < n; i++)						\
      {									\
	r = strndup (p, 4);						\
	p = (i & 1) == s_iter ? s : r;					\
      }									\
    *rsize = __builtin_object_size (r, mode);				\
    return __builtin_object_size (p, mode);				\
  }

LATE_UNKNOWN_LOOP (0, odd, 1)
LATE_UNKNOWN_LOOP (1, odd, 1)
LATE_UNKNOWN_LOOP (0, even, 0)
LATE_UNKNOWN_LOOP (1, even, 0)

/* Same as above, with only the PHI's size asked for.  */
#define LATE_UNKNOWN_PHI_LOOP(mode)					\
  __attribute__ ((noipa)) size_t					\
  late_unknown_phi_loop_##mode (int n, char *s)				\
  {									\
    char *p = buf;							\
    for (int i = 0; i < n; i++)						\
      {									\
	p = strndup (p, 4);						\
	if (i & 1)							\
	  p = s;							\
      }									\
    return __builtin_object_size (p, mode);				\
  }

LATE_UNKNOWN_PHI_LOOP (0)
LATE_UNKNOWN_PHI_LOOP (1)

/* The size of a strdup result is the length of the string plus 1, which only a
   dynamic size can use.  */
__attribute__ ((noipa)) size_t
strdup_loop (int n)
{
  char *p = buf;
  for (int i = 0; i < n; i++)
    p = strdup (p);
  return __builtin_object_size (p, 0);
}

/* A source with no NUL in its 8 bytes gets duplicated in full with a NUL added,
   which makes 9 the maximum for a result.  */
__attribute__ ((noipa)) size_t
same_bound_loop (int n)
{
  char *p = buf;
  for (int i = 0; i < n; i++)
    p = strndup (p, 8);
  return __builtin_object_size (p, 0);
}

/* A bound this big only leaves the size of the source to limit a result, and
   adding 1 to SIZE_MAX must not get in the way of that.  */
__attribute__ ((noipa)) size_t
huge_bound_loop (int n)
{
  char *p = buf, *q = buf;
  for (int i = 0; i < n; i++)
    {
      p = strndup (p, -1);
      q = strndup (q, -2);
    }
  return __builtin_object_size (p, 0) + __builtin_object_size (q, 0);
}

#endif /* SKIP_STRNDUP */

int
main (void)
{
#ifndef SKIP_STRNDUP
  for (int n = 0; n < 4; n++)
    {
      if (self_loop_0_4 (n) != SIZE (8, n ? 5 : 8))
	FAIL ();
      if (self_loop_1_4 (n) != SIZE (8, n ? 5 : 8))
	FAIL ();
      if (self_loop_2_4 (n) != SIZE (1, n ? 1 : 8))
	FAIL ();
      if (self_loop_3_4 (n) != SIZE (1, n ? 1 : 8))
	FAIL ();
      if (self_loop_0_16 (n) != 8)
	FAIL ();
      if (self_loop_1_16 (n) != 8)
	FAIL ();
      if (self_loop_2_16 (n) != SIZE (1, n ? 1 : 8))
	FAIL ();
      if (self_loop_3_16 (n) != SIZE (1, n ? 1 : 8))
	FAIL ();
      if (result_loop_0 (n) != 5)
	FAIL ();
      if (result_loop_1 (n) != 5)
	FAIL ();
      if (result_loop_2 (n) != 1)
	FAIL ();
      if (result_loop_3 (n) != 1)
	FAIL ();
      /* Each iteration skips a byte of what the previous one returned, which
	 still leaves a source of 4 bytes.  With no NUL in them, a result has
	 all 4 and a NUL.  */
      if (offset_loop_0 (n) != SIZE (8, n ? 5 : 8))
	FAIL ();
      if (offset_loop_1 (n) != SIZE (8, n ? 5 : 8))
	FAIL ();
      /* The source shrinks by a byte each time, and BUF is the biggest.  */
      if (negative_offset_loop_0 (n) != SIZE (8, 8 - n))
	FAIL ();
      if (negative_offset_loop_1 (n) != SIZE (8, 8 - n))
	FAIL ();
      /* A result only has room for the 4 bytes of B, so a second iteration
	 would read past its end.  Any maximum of at least 4 bytes is valid for
	 it, and a pointer past the end of a result has no required size.  */
      if (n < 2)
	{
	  if (member_loop_0 (n) < 4)
	    FAIL ();
	  if (member_loop_1 (n) < 4)
	    FAIL ();
	  member_query_loop_0 (n);
	  member_query_loop_1 (n);
	}
      if (two_calls_loop (n) != SIZE (8, n ? 5 : 8))
	FAIL ();
      /* Only the odd iterations call strndup.  */
      if (diamond_loop (n) != SIZE (8, n < 2 ? 8 : 5))
	FAIL ();
      if (pass_through_loop (n) != SIZE (8, n ? 5 : 8))
	FAIL ();
      if (query_in_loop (n) != SIZE (n ? 8 : 0, n < 2 ? (n ? 8 : 0) : 5))
	FAIL ();
      if (variable_bound_loop (n, 4) != SIZE (-1, n ? 5 : 8))
	FAIL ();
      if (variable_bound_loop (n, 16) != SIZE (-1, 8))
	FAIL ();
      if (variable_source_loop (n, 16) != SIZE (-1, n ? 5 : 16))
	FAIL ();
      size_t rsize;
      if (unknown_source_loop (n, buf, &rsize) != (size_t) -1)
	FAIL ();
      /* An unknown size is a valid maximum too.  */
      if (n && rsize < 5)
	FAIL ();
      /* S has no known size, which leaves a result with at most the 4 bytes of
	 the bound and a NUL.  */
      if (late_unknown_odd_0 (n, buf, &rsize) != (size_t) -1
	  || rsize != SIZE (8, n ? 5 : 8))
	FAIL ();
      if (late_unknown_odd_1 (n, buf, &rsize) != (size_t) -1
	  || rsize != SIZE (8, n ? 5 : 8))
	FAIL ();
      if (late_unknown_even_0 (n, buf, &rsize) != (size_t) -1
	  || rsize != SIZE (8, n ? 5 : 8))
	FAIL ();
      if (late_unknown_even_1 (n, buf, &rsize) != (size_t) -1
	  || rsize != SIZE (8, n ? 5 : 8))
	FAIL ();
      if (late_unknown_phi_loop_0 (n, buf) != (size_t) -1)
	FAIL ();
      if (late_unknown_phi_loop_1 (n, buf) != (size_t) -1)
	FAIL ();
      if (strdup_loop (n) != SIZE (-1, n ? 4 : 8))
	FAIL ();
      if (same_bound_loop (n) != SIZE (9, n ? 9 : 8))
	FAIL ();
      if (huge_bound_loop (n) != 16)
	FAIL ();
    }
#endif

  DONE ();
}
