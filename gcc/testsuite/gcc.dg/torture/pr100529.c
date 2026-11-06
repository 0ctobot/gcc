/* PR middle-end/100529 */
/* { dg-do compile } */
/* { dg-options "-std=gnu11 -w -fchecking" } */

#include <stdarg.h>

void bar ();
int map ();

/* The original reproducer: a cast to a union with a variable-length member
   whose size is computed by a call.  */
void
cast_ptr (char a)
{
  union C { int d[map ()]; char *e; };
  bar ((union C) &a);
}

void
cast_arg (int n, int v)
{
  union U { char a[n]; int e; };
  bar ((union U) v);
}

void
cast_anon (int v)
{
  bar ((union { char a[map ()]; int e; }) v);
}

void
cast_auto (int n, int v)
{
  union U { char a[n]; int e; };
  __auto_type u = (union U) v;
  bar (u);
}

void
cast_cond (int n, int c, int v)
{
  union U { char a[n]; int e; };
  bar (c ? (union U) v : (union U) 1);
}

void
cond_arg (int n, int c)
{
  struct S { char a[n]; } x, y;
  bar (c ? x : y);
}

char
cond_member (int n, int c)
{
  struct S { char a[n]; } x, y;
  return (c ? x : y).a[0];
}

char *
cond_decay (int n, int c)
{
  struct S { char a[n]; } x, y;
  char *p = (c ? x : y).a;
  return p;
}

void
cond_nested (int n, int c, int d)
{
  struct S { char a[n]; } x, y, z;
  bar (c ? (d ? x : y) : z);
}

void
cond_comma (int n, int c)
{
  struct S { char a[n]; } x, y;
  bar ((c, c ? x : y));
}

void
cond_volatile (int n, int c)
{
  struct S { char a[n]; };
  volatile struct S x, y;
  bar (c ? x : y);
}

void
cond_const (int n, int c)
{
  struct S { char a[n]; };
  const struct S x, y;
  bar (c ? x : y);
}

void
cond_auto (int n, int c)
{
  struct S { char a[n]; } x, y;
  __auto_type z = c ? x : y;
  bar (z);
}

void
cond_loop (int n, int c)
{
  struct S { char a[n]; } x, y;
  for (int i = 0; i < n; i++)
    bar (c ? x : y);
}

void
cond_stmt_expr (int n, int c)
{
  struct S { char a[n]; } x, y;
  bar (({ __auto_type t = c ? x : y; t; }));
}

void
auto_copy (int n)
{
  struct S { char a[n]; } x;
  __auto_type z = x;
  bar (z);
}

void
vaarg_arg (int n, ...)
{
  struct S { char a[n]; };
  va_list ap;
  va_start (ap, n);
  bar (va_arg (ap, struct S));
  va_end (ap);
}

char
vaarg_member (int n, ...)
{
  struct S { char a[n]; };
  va_list ap;
  char r;
  va_start (ap, n);
  r = va_arg (ap, struct S).a[0];
  va_end (ap);
  return r;
}

void
nested_arg (int n)
{
  struct S { char a[n]; };
  struct S g (void) { struct S s; return s; }
  bar (g ());
}

void
nested_auto (int n)
{
  struct S { char a[n]; };
  struct S g (void) { struct S s; return s; }
  __auto_type z = g ();
  bar (z);
}

void
nested_inline (int n)
{
  struct S { char a[n]; };
  inline __attribute__ ((always_inline)) struct S
  g (void)
  {
    struct S s;
    s.a[0] = 1;
    return s;
  }
  bar (g ());
}

void
fnptr_arg (int n, int c, void (*fp) ())
{
  struct S { char a[n]; } x, y;
  fp (c ? x : y);
}

/* The value of an assignment to a volatile destination.  */
void
volatile_call (int n, volatile void *vp)
{
  struct S { char a[n]; };
  struct S g (void) { struct S s; return s; }
  volatile struct S *p = vp;
  bar (*p = g ());
}

char
volatile_call_member (int n, volatile void *vp)
{
  struct S { char a[n]; };
  struct S g (void) { struct S s; return s; }
  volatile struct S *p = vp;
  return (*p = g ()).a[0];
}

void
volatile_call_chain (int n, volatile void *vp, volatile void *vq)
{
  struct S { char a[n]; };
  struct S g (void) { struct S s; return s; }
  volatile struct S *p = vp, *q = vq;
  bar (*p = *q = g ());
}

void
volatile_vaarg (int n, volatile void *vp, ...)
{
  struct S { char a[n]; };
  va_list ap;
  va_start (ap, vp);
  volatile struct S *p = vp;
  bar (*p = va_arg (ap, struct S));
  va_end (ap);
}

/* Temporaries in a statement that can be left by a jump.  */
void
jump_arm (int n, int c)
{
  struct S { char a[n]; } x, y;
  for (int i = 0; i < n; i++)
    bar (c ? x : ({ if (i) continue; y; }));
}

void
jump_asm (int n, int c)
{
  struct S { char a[n]; } x, y;
  bar (c ? x : y, ({ asm goto ("" : : : : out); 0; }));
 out:
  bar ();
}

void
jump_computed (int n, int c, void *p)
{
  struct S { char a[n]; } x, y;
  bar (c ? x : y, ({ goto *p; 0; }));
 out:
  bar (&&out);
}

int
jump_pred (int n, int c)
{
  struct S { char a[n]; } x, y;
  if ((c ? x : y).a[0] + ({ if (n) goto out; 1; }))
    {
      bar ();
      bar ();
    }
 out:
  return 0;
}

/* A label in an arm of an if statement can be reached without evaluating the
   predicate.  */
void
label_arm (int n, int c, int d)
{
  struct S { char a[n]; } x, y;
  if (d)
    goto out;
  if ((c ? x : y).a[0])
    {
    out:
      bar ();
    }
}

void
label_case (int n, int c, int d)
{
  struct S { char a[n]; } x, y;
  switch (d)
    {
    case 0:
      if ((c ? x : y).a[0])
	{
	case 1:
	  bar ();
	}
    }
}

/* The scope for the predicate's temporaries closes before the branch here, and
   a goto leaves that scope.  */
int
label_jump_pred (int n, int c, int d)
{
  struct S { char a[n]; } x, y;
  if (d)
    goto in;
  if ((c ? x : y).a[0] + ({ if (n) goto out; 1; }))
    {
    in:
      bar ();
    }
 out:
  return 0;
}

int
label_jump_switch (int n, int c, int d)
{
  struct S { char a[n]; } x, y;
  switch ((c ? x : y).a[0] + ({ if (d) goto out; 1; }))
    {
    case 1:
      bar ();
    }
 out:
  return 0;
}

/* The loop pragmas expect their annotation to reach the loop's condition.  */
void
pragma_loop (int n, int c)
{
  struct S { char a[n]; } x, y;
#pragma GCC ivdep
  for (int i = 0; i < n + (c ? x : y).a[0]; i++)
    bar ();
#pragma GCC unroll 4
  for (int i = 0; i < n + (c ? x : y).a[0]; i++)
    bar ();
#pragma GCC novector
  for (int i = 0; i < n + (c ? x : y).a[0]; i++)
    bar ();
}

/* The value of a nested function's return statement.  */
void
nested_ret_cond (int n, int c)
{
  struct S { char a[n]; } x, y;
  struct S g (void) { return c ? x : y; }
  bar (g ());
}
