/* PR middle-end/100529 */
/* { dg-do run } */
/* { dg-options "-std=gnu11 -w -fchecking" } */

#include <stdarg.h>

static int size_calls;

__attribute__ ((noipa)) int
get_size (int n)
{
  size_calls++;
  return n;
}

/* Check the bytes of a variable-sized aggregate passed by value.  */
__attribute__ ((noipa)) void
check_struct (int n, int base, ...)
{
  struct S { char a[n]; } s;
  va_list ap;

  va_start (ap, base);
  s = va_arg (ap, struct S);
  va_end (ap);
  for (int i = 0; i < n; i++)
    if (s.a[i] != (char) (base + i))
      __builtin_abort ();
}

/* Check the leading int of a variable-sized union passed by value.  */
__attribute__ ((noipa)) void
check_union (int n, int v, ...)
{
  union U { char a[n]; int e; } u;
  va_list ap;

  va_start (ap, v);
  u = va_arg (ap, union U);
  va_end (ap);
  if (u.e != v)
    __builtin_abort ();
}

/* Forward the first variadic argument with va_arg used as an rvalue.  */
__attribute__ ((noipa)) void
forward (int n, int base, ...)
{
  struct S { char a[n]; };
  va_list ap;

  va_start (ap, base);
  check_struct (n, base, va_arg (ap, struct S));
  if (va_arg (ap, struct S).a[n - 1] != (char) (base + 100 + n - 1))
    __builtin_abort ();
  va_end (ap);
}

__attribute__ ((noipa)) void
test_cond (int n, int c)
{
  struct S { char a[n]; } x, y;
  int xbase = 10, ybase = 50;

  for (int i = 0; i < n; i++)
    {
      x.a[i] = xbase + i;
      y.a[i] = ybase + i;
    }

  check_struct (n, c ? xbase : ybase, c ? x : y);
  check_struct (n, c ? ybase : xbase, !c ? x : (0, y));
  if ((c ? x : y).a[n - 1] != (char) ((c ? xbase : ybase) + n - 1))
    __builtin_abort ();
  check_struct (n, c ? xbase : ybase, n > 0 ? (c ? x : y) : x);

  __auto_type z = c ? x : y;
  if (sizeof z != (unsigned long) n)
    __builtin_abort ();
  check_struct (n, c ? xbase : ybase, z);

  /* The temporary is a copy, so writing it can't change the source.  */
  z.a[0] = 99;
  if ((c ? x.a[0] : y.a[0]) != (char) (c ? xbase : ybase))
    __builtin_abort ();

  __auto_type w = x;
  w.a[0] = 98;
  if (x.a[0] != (char) xbase || w.a[n - 1] != x.a[n - 1])
    __builtin_abort ();

  check_struct (n, c ? xbase : ybase, ({ __auto_type t = c ? x : y; t; }));
}

__attribute__ ((noipa)) void
test_cast (int n, int v)
{
  union U { char a[n]; int e; };

  check_union (n, v, (union U) v);
  check_union (n, v + 1, n > 0 ? (union U) (v + 1) : (union U) v);

  __auto_type u = (union U) v;
  if (u.e != v || sizeof u != (unsigned long) n)
    __builtin_abort ();
}

/* The size of an anonymous type must only be evaluated once.  */
__attribute__ ((noipa)) void
test_anon_size (int n, int v)
{
  size_calls = 0;
  check_union (n, v, (union { char a[get_size (n)]; int e; }) v);
  if (size_calls != 1)
    __builtin_abort ();

  size_calls = 0;
  __auto_type u = (union { char a[get_size (n)]; int e; }) v;
  if (u.e != v || sizeof u != (unsigned long) n || size_calls != 1)
    __builtin_abort ();
}

__attribute__ ((noipa)) void
test_va_arg (int n, int base)
{
  struct S { char a[n]; } x, y;

  for (int i = 0; i < n; i++)
    {
      x.a[i] = base + i;
      y.a[i] = base + 100 + i;
    }
  forward (n, base, x, y);
}

/* A nested function returning a variable-sized type must only run once, even
   when its value initializes __auto_type.  */
__attribute__ ((noipa)) void
test_nested_ret (int n, int base)
{
  struct S { char a[n]; };
  int calls = 0;
  struct S g (void)
    {
      struct S s;
      calls++;
      for (int i = 0; i < n; i++)
	s.a[i] = base + i;
      return s;
    }

  check_struct (n, base, g ());
  if (calls != 1)
    __builtin_abort ();

  calls = 0;
  __auto_type z = g ();
  if (calls != 1 || sizeof z != (unsigned long) n)
    __builtin_abort ();
  check_struct (n, base, z);
}

/* Assigning to a volatile destination must store the value and yield it.  */
__attribute__ ((noipa)) void
vaarg_volatile (int n, int base, volatile void *vp, ...)
{
  struct S { char a[n]; };
  volatile struct S *p = vp;
  va_list ap;

  va_start (ap, vp);
  check_struct (n, base, *p = va_arg (ap, struct S));
  va_end (ap);
}

__attribute__ ((noipa)) void
test_volatile (int n, int base)
{
  struct S { char a[n]; } dst;
  volatile struct S *p = (volatile struct S *) &dst;
  int calls = 0;
  struct S g (void)
    {
      struct S s;
      calls++;
      for (int i = 0; i < n; i++)
	s.a[i] = base + i;
      return s;
    }

  check_struct (n, base, *p = g ());
  if (calls != 1 || dst.a[n - 1] != (char) (base + n - 1))
    __builtin_abort ();

  __builtin_memset (&dst, 0, n);
  if ((*p = g ()).a[n - 1] != (char) (base + n - 1) || calls != 2
      || dst.a[0] != (char) base)
    __builtin_abort ();

  struct S src;
  src = g ();
  __builtin_memset (&dst, 0, n);
  vaarg_volatile (n, base, &dst, src);
  if (dst.a[n - 1] != (char) (base + n - 1))
    __builtin_abort ();
}

#define LOOPS 8

static void *frames[1024];

/* Check that all calls from the line SITE are made with the same stack pointer,
   which would change if the temporaries weren't reclaimed.  */
__attribute__ ((noipa)) int
check_frame (int site, ...)
{
  void *frame = __builtin_frame_address (0);

  if (!frames[site])
    frames[site] = frame;
  else if (frames[site] != frame)
    __builtin_abort ();
  return 1;
}

#define FRAME(...) check_frame (__LINE__, __VA_ARGS__)

__attribute__ ((noipa)) void
vaarg_loop (int n, ...)
{
  struct S { char a[n]; };
  va_list ap;

  va_start (ap, n);
  for (int i = 0; i < 4; i++)
    FRAME (va_arg (ap, struct S));
  va_end (ap);
}

static void
cleanup_int (int *p)
{
  ++*p;
}

/* The temporaries must be reclaimed on every loop iteration, regardless of the
   loop's form and of where in the loop they get created.  */
__attribute__ ((noipa)) void
test_loop (int n, int c)
{
  struct S { char a[n]; } x, y;
  union U { char a[n]; int e; };
  struct S g (void) { return x; }
  int i, r = 0;

  __builtin_memset (&x, 1, n);
  __builtin_memset (&y, 2, n);
  __builtin_memset (frames, 0, sizeof frames);

  for (i = 0; i < LOOPS; i++)
    FRAME (c ? x : y);

  for (i = 0; i < LOOPS; i++)
    {
      __auto_type z = c ? x : y;
      FRAME (z);
    }

  i = LOOPS;
  while (i--)
    FRAME ((union U) i);

  for (i = 0; i < LOOPS; i++)
    FRAME (g ());

  vaarg_loop (n, x, x, x, x);

  size_calls = 0;
  for (i = 0; i < LOOPS; i++)
    FRAME ((union { char a[get_size (n)]; int e; }) i);
  if (size_calls != LOOPS)
    __builtin_abort ();

  for (i = 0; FRAME (c ? x : y) && i < LOOPS; i++)
    ;

  i = 0;
  while (FRAME (c ? x : y) + i++ < LOOPS)
    ;

  i = 0;
  do
    r++;
  while (FRAME ((union U) i) + i++ < LOOPS);

  for (i = 0; i < LOOPS; i += FRAME (c ? x : y))
    ;

  i = 0;
again:
  FRAME (c ? x : y);
  if (++i < LOOPS)
    goto again;

  for (i = 0; i < LOOPS; i++)
    if (FRAME (c ? x : y))
      r++;

  for (i = 0; i < LOOPS; i++)
    if (FRAME (c ? x : y))
      {
	r++;
	r++;
      }

  for (i = 0; i < LOOPS; i++)
    switch (FRAME (c ? x : y))
      {
      case 1:
	r++;
	break;
      default:
	__builtin_abort ();
      }

  for (i = 0; i < LOOPS; i++)
    if (c)
      FRAME (c ? x : y);
    else
      FRAME ((union U) i);

  /* The arms of an if statement with a temporary in its predicate are
     statements that reclaim their own temporaries.  */
  for (i = 0; i < LOOPS; i++)
    if (FRAME (c ? x : y))
      for (int j = 0; j < LOOPS; j++)
	FRAME (c ? x : y);

  /* Each statement must reclaim its own temporaries, even within one scope.  */
  for (i = 0; i < LOOPS; i++)
    {
      int unused __attribute__ ((cleanup (cleanup_int))) = 0;
      FRAME (c ? x : y); FRAME (c ? x : y);
    }

  /* The same goes for statements within a statement expression.  */
  FRAME (({ for (i = 0; i < LOOPS; i++)
	      FRAME (c ? x : y);
	    x; }));
}

/* Jumping out of a statement must reclaim the temporaries created so far.  */
__attribute__ ((noipa)) int
jump_return (int n, int c, int d)
{
  struct S { char a[n]; } x, y;
  char *p;

  __builtin_memset (&x, 1, n);
  __builtin_memset (&y, 2, n);
  FRAME ((p = (c ? x : y).a, ({ if (d) return 1; 0; })));
  return 0;
}

__attribute__ ((noipa)) void
test_jump_out (int n, int c, int d)
{
  struct S { char a[n]; } x, y;
  int i, r;
  char *p;

  __builtin_memset (&x, 1, n);
  __builtin_memset (&y, 2, n);
  __builtin_memset (frames, 0, sizeof frames);

  for (int k = 0; k < LOOPS; k++)
    {
      FRAME (x);
      for (i = 0; i < 2; i++)
	FRAME ((p = (c ? x : y).a, ({ if (d) continue; 0; })));

      FRAME (x);
      for (i = 0; i < 2; i++)
	FRAME ((p = (c ? x : y).a, ({ if (d) break; 0; })));

      FRAME (x);
      FRAME ((p = (c ? x : y).a, ({ if (d) goto out1; 0; })));
    out1:
      FRAME (x);

      /* The values of these predicates are read after the temporaries are
	 reclaimed, on a path that's only reachable when no jump is taken.  */
      r = 0;
      if ((c ? x : y).a[0] + ({ if (d) goto out2; x.a[0]; }) == (c ? 2 : 3))
	{
	  r += 1;
	  r += 2;
	}
      else
	__builtin_abort ();
      if (r != 3)
	__builtin_abort ();
    out2:
      FRAME (x);
      switch ((c ? x : y).a[0] + ({ if (d) goto out3; x.a[0]; }))
	{
	case 2:
	  if (!c)
	    __builtin_abort ();
	  break;
	case 3:
	  if (c)
	    __builtin_abort ();
	  break;
	default:
	  __builtin_abort ();
	}
    out3:
      FRAME (x);
      if (jump_return (n, c, d) != d)
	__builtin_abort ();
    }
}

/* Jumping past a statement that creates a temporary must not leave a stack
   restore running without its stack save.  */
__attribute__ ((noipa)) int
test_jump_in (int n, int c, int d)
{
  struct S { char a[n]; } x, y;
  int i, r = 0;

  __builtin_memset (&x, 1, n);
  __builtin_memset (&y, 2, n);
  __builtin_memset (frames, 0, sizeof frames);

  for (int k = 0; k < LOOPS; k++)
    {
      FRAME (x);
      if (d)
	goto l1;
      {
	int v;
	r += FRAME (c ? x : y);
      l1:
	v = 1;
	r += v;
      }

      FRAME (x);
      if (d)
	goto l2;
      if (c)
	FRAME (c ? x : y);
      else
	{
	l2:
	  r++;
	  r++;
	}

      FRAME (x);
      if (d)
	goto l3;
      if (FRAME (c ? x : y))
	{
	l3:
	  r++;
	  r++;
	}

      FRAME (x);
      if (d)
	goto l4;
      r++;
    l4:
      FRAME (c ? x : y);

      FRAME (x);
      i = 0;
      switch (d)
	{
	case 0:
	  for (; i < 2; i++)
	    {
	      FRAME (c ? x : y);
	    case 1:
	      r++;
	    }
	}
      FRAME (x);
    }
  return r;
}

/* Overwrite the stack below the caller's stack pointer.  */
__attribute__ ((noipa)) void
clobber (int n)
{
  char buf[n * 8 + 64];

  __builtin_memset (buf, 0x55, sizeof buf);
  __asm__ volatile ("" : : "r" (buf) : "memory");
}

__attribute__ ((noipa)) void
check_char (char want, char have)
{
  if (want != have)
    __builtin_abort ();
}

__attribute__ ((noipa)) void
check_tmp (int n, const char *p, char want)
{
  clobber (n);
  check_char (want, p[n - 1]);
}

__attribute__ ((noipa)) void *
call_frame (void)
{
  return __builtin_frame_address (0);
}

/* A jump to a label before an __auto_type declaration runs the declaration.  It
   mustn't skip the stack save for the temporary of the initializer, since the
   restore at the end of the block then sets the stack pointer to garbage.  */
__attribute__ ((noipa)) int
test_jump_auto (int n, int c, int d)
{
  struct S { char a[n]; } x, y;
  struct S g (void) { return c ? x : y; }
  char want = c ? 1 : 2;
  void *frame = call_frame ();
  int r = 0;

  __builtin_memset (&x, 1, n);
  __builtin_memset (&y, 2, n);

  if (d)
    goto l1;
  r++;
  {
    int i;
  l1:
    i = 1;
    __auto_type z = c ? x : y;
    check_char (want, z.a[n - 1]);
    r += i;
  }
  if (call_frame () != frame)
    __builtin_abort ();

  if (d)
    goto l2;
  r++;
  {
    int i;
  l2:
    i = 1;
    __auto_type z = g ();
    check_char (want, z.a[n - 1]);
    r += i;
  }
  if (call_frame () != frame)
    __builtin_abort ();

  switch (d)
    {
    case 0:
      r++;
    case 1:;
      __auto_type z = c ? x : y;
      check_char (want, z.a[n - 1]);
      r++;
    }
  if (call_frame () != frame)
    __builtin_abort ();

  return r;
}

__attribute__ ((noipa)) int
compare (const char *p, const char *q, int n)
{
  return __builtin_memcmp (p, q, n);
}

__attribute__ ((noipa)) char
ret_member (int n, int c)
{
  struct S { char a[n]; } x, y;

  __builtin_memset (&x, 1, n);
  __builtin_memset (&y, 2, n);
  return (c ? x : y).a[n - 1];
}

/* A temporary's storage must remain valid until the end of its full expression,
   not just until the end of the subexpression that creates it.  */
__attribute__ ((noipa)) void
test_lifetime (int n, int c)
{
  struct S { char a[n]; } x, y;
  char want = c ? 1 : 2;
  char *p = 0;
  int r = 0;

  __builtin_memset (&x, 1, n);
  __builtin_memset (&y, 2, n);

  check_char (want, (p = (c ? x : y).a, clobber (n), p[n - 1]));

  check_char (want, ((n ? (void) (p = (c ? x : y).a) : (void) (p = x.a)),
		     clobber (n), p[n - 1]));

  check_char (want, (n && (p = (c ? x : y).a)[0]
		     ? (clobber (n), p[n - 1]) : 0));

  if ((p = (c ? x : y).a)[0] && (clobber (n), p[n - 1]) == want)
    r++;
  if (r != 1)
    __builtin_abort ();

  if (!(p = (c ? x : y).a)[0] || (clobber (n), p[n - 1]) != want)
    __builtin_abort ();

  if ((p = (c ? x : y).a, clobber (n), p[0]) == want
      && (clobber (n), p[n - 1]) == want)
    {
      r++;
      r++;
    }
  if (r != 3)
    __builtin_abort ();

  /* A void ?: operator can read its predicate's temporary in its arms.  */
  (p = (c ? x : y).a)[0] ? check_tmp (n, p, want) : __builtin_abort ();
  !(p = (c ? x : y).a)[0] ? __builtin_abort () : check_tmp (n, p, want);
  (p = (c ? x : y).a)[0] ? ({ check_tmp (n, p, want); }) : __builtin_abort ();
  (p = (c ? x : y).a)[0] ? ({ for (int i = 0; i < 2; i++)
				check_tmp (n, p, want); }) : __builtin_abort ();

  /* Temporaries can't overlap, even when one is created in a ?: arm.  */
  if (!compare ((n ? (c ? x : y).a : x.a), (c ? y : x).a, n))
    __builtin_abort ();

  check_char (want, ret_member (n, c));
}

__attribute__ ((noipa)) void *
keep_ptr (void *p, ...)
{
  return p;
}

/* Memory from a user's alloca must outlive a statement with a temporary.  */
__attribute__ ((noipa)) void
test_alloca (int n, int c)
{
  struct S { char a[n]; } x, y;
  char *p[LOOPS];

  __builtin_memset (&x, 1, n);
  __builtin_memset (&y, 2, n);

  for (int i = 0; i < LOOPS; i++)
    {
      p[i] = keep_ptr (__builtin_alloca (n), c ? x : y);
      __builtin_memset (p[i], i, n);
    }
  clobber (n);
  for (int i = 0; i < LOOPS; i++)
    for (int j = 0; j < n; j++)
      check_char (i, p[i][j]);
}

int
main (void)
{
  for (int c = 0; c < 2; c++)
    {
      test_cond (17, c);
      test_cond (4096, c);
      test_loop (256, c);
      test_lifetime (256, c);
      test_alloca (256, c);
      for (int d = 0; d < 2; d++)
	{
	  test_jump_out (256, c, d);
	  test_jump_in (256, c, d);
	  if (test_jump_auto (256, c, d) != (d ? 3 : 6))
	    __builtin_abort ();
	}
    }
  test_cast (8, 0x12345678);
  test_cast (1024, 0x12345678);
  test_anon_size (16, 0x1234);
  test_va_arg (24, 3);
  test_nested_ret (32, 5);
  test_volatile (40, 7);
  return 0;
}
