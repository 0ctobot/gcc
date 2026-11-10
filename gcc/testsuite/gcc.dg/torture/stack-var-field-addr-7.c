/* { dg-do run } */

/* Same as stack-var-field-addr-4.c, except that the const call returns the
   address inside a complex or vector value, so q only gets it back through a
   REALPART_EXPR, BIT_FIELD_REF or VIEW_CONVERT_EXPR of the call's result.  foo
   uses __builtin_add_overflow, which becomes the const internal call
   .ADD_OVERFLOW.  q still has to count as a mention of v and w.  */

typedef __UINTPTR_TYPE__ uptr;
typedef uptr v2 __attribute__((vector_size (2 * sizeof (uptr))));
typedef unsigned char vb __attribute__((vector_size (sizeof (uptr))));

struct S { long pad; int f; int g; };

volatile int sink;

__attribute__((noipa)) void
use (int *p)
{
  sink += *p;
}

__attribute__((noipa)) void
set (int *p, int val)
{
  *p = val;
}

__attribute__((const, noipa)) _Complex __UINTPTR_TYPE__
cfld (uptr p)
{
  _Complex __UINTPTR_TYPE__ r;
  __real__ r = p + __builtin_offsetof (struct S, f);
  __imag__ r = 0;
  return r;
}

__attribute__((const, noipa)) v2
vfld (uptr p)
{
  return (v2) { p + __builtin_offsetof (struct S, f), 0 };
}

__attribute__((const, noipa)) vb
bfld (uptr p)
{
  return (vb) (p + __builtin_offsetof (struct S, f));
}

__attribute__((noipa)) int
foo (int sel, int n)
{
  struct S y;
  int first = 1;

  for (int i = 0; __builtin_expect_with_probability (i < n, 1, 0.5); i++)
    {
      {
	struct S v, w;
	struct S *p = sel ? &v : &w;
	uptr a;
	if (__builtin_add_overflow ((uptr) p,
				    (uptr) __builtin_offsetof (struct S, f),
				    &a))
	  __builtin_abort ();
	set ((int *) a, i + 100);
      }
      if (first)
	{
	  y.f = 42;
	  first = 0;
	}
      use (&y.f);
    }
  return first ? -1 : y.f;
}

__attribute__((noipa)) int
bar (int sel, int n)
{
  struct S y;
  int first = 1;

  for (int i = 0; __builtin_expect_with_probability (i < n, 1, 0.5); i++)
    {
      {
	struct S v, w;
	struct S *p = sel ? &v : &w;
	int *q = (int *) __real__ cfld ((uptr) p);
	set (q, i + 100);
      }
      if (first)
	{
	  y.f = 42;
	  first = 0;
	}
      use (&y.f);
    }
  return first ? -1 : y.f;
}

__attribute__((noipa)) int
baz (int sel, int n)
{
  struct S y;
  int first = 1;

  for (int i = 0; __builtin_expect_with_probability (i < n, 1, 0.5); i++)
    {
      {
	struct S v, w;
	struct S *p = sel ? &v : &w;
	int *q = (int *) vfld ((uptr) p)[0];
	set (q, i + 100);
      }
      if (first)
	{
	  y.f = 42;
	  first = 0;
	}
      use (&y.f);
    }
  return first ? -1 : y.f;
}

__attribute__((noipa)) int
qux (int sel, int n)
{
  struct S y;
  int first = 1;

  for (int i = 0; __builtin_expect_with_probability (i < n, 1, 0.5); i++)
    {
      {
	struct S v, w;
	struct S *p = sel ? &v : &w;
	int *q = (int *) (uptr) bfld ((uptr) p);
	set (q, i + 100);
      }
      if (first)
	{
	  y.f = 42;
	  first = 0;
	}
      use (&y.f);
    }
  return first ? -1 : y.f;
}

int
main (void)
{
  for (int sel = 0; sel < 2; sel++)
    {
      if (foo (sel, 3) != 42)
	__builtin_abort ();
      if (bar (sel, 3) != 42)
	__builtin_abort ();
      if (baz (sel, 3) != 42)
	__builtin_abort ();
      if (qux (sel, 3) != 42)
	__builtin_abort ();
    }
  return 0;
}
