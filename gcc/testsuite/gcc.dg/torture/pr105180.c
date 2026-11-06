/* PR c/105180: the size expressions of old-style parameter declarations are
   evaluated on entry to the function, just like those of a prototype.  */
/* { dg-do run } */
/* { dg-require-effective-target libatomic_available } */
/* { dg-additional-options "-std=gnu17 -w" } */

extern void abort (void);

int global;
_Atomic int atomic_size = 5;

__attribute__ ((noipa)) int
func (void)
{
  return ++global;
}

/* An array parameter decays to a pointer, so nothing but the evaluation on
   entry can run the side effects of its size.  */

__attribute__ ((noipa)) void
array_static (s, c)
     char *s;
     char c[static func ()];
{
}

__attribute__ ((noipa)) void
array (c)
     char c[func ()];
{
}

__attribute__ ((noipa)) void
several (c, d, e)
     char c[func ()];
     char d[func ()], e[func ()];
{
}

__attribute__ ((noipa)) void
two_dimensions (c)
     char c[func ()][func ()];
{
}

__attribute__ ((noipa)) void
pointer_to_vla (p)
     char (*p)[func ()];
{
}

__attribute__ ((noipa)) void
register_array (c, d)
     char c[func ()];
     register char d[func ()];
{
}

__attribute__ ((noipa)) void
atomic_op (c)
     char c[atomic_size += 2];
{
}

__attribute__ ((noipa)) void
compound_literal (c)
     char c[(int) { func () }];
{
}

__attribute__ ((noipa)) void
sizeof_vla (c)
     char c[sizeof (char[func ()])];
{
}

/* The body sees the parameter after the side effect of the size.  */

__attribute__ ((noipa)) int
increment (c, n)
     int n;
     char c[n++];
{
  return n;
}

__attribute__ ((noipa)) int
increment_promoted (c, n)
     char n;
     char c[n++];
{
  return n;
}

__attribute__ ((noipa)) int
increment_twice (c, d, n)
     int n;
     char c[n++];
     char d[n++];
{
  return n;
}

/* The sizes are evaluated in the order of the declarations, not in the order
   of the identifier list.  */

__attribute__ ((noipa)) int
order (a, b, n)
     int n;
     char b[n *= 2];
     char a[n += 3];
{
  return n;
}

/* A size is evaluated before the first statement of the body.  */

__attribute__ ((noipa)) int
before_body (n, c)
     int n;
     char c[global = n];
{
  int r = global;
  global = 0;
  return r;
}

/* A specifier shared by several declarators is evaluated once.  */

__attribute__ ((noipa)) int
shared_specifier (n, p, q)
     int n;
     __typeof__ (char[n++]) *p, *q;
{
  return n;
}

/* The size of a type that survives the decay is fixed on entry, even without a
   side effect in it, so later changes of the parameter don't affect it.  */

__attribute__ ((noipa)) int
pointer_size (n, p)
     int n;
     char (*p)[n];
{
  n = 5;
  return sizeof (*p);
}

__attribute__ ((noipa)) int
decayed_size (n, p)
     int n;
     char p[][n];
{
  n = 5;
  return sizeof (*p);
}

__attribute__ ((noipa)) int
typeof_size (n, p)
     int n;
     __typeof__ (char[n]) *p;
{
  n = 5;
  return sizeof (*p);
}

__attribute__ ((noipa)) int
struct_size (n, p)
     int n;
     struct { char x[n]; } *p;
{
  n = 5;
  return sizeof (*p);
}

__attribute__ ((noipa)) int
shared_size (n, p, q)
     int n;
     __typeof__ (char[n]) *p, (*q)[2];
{
  n = 9;
  return sizeof (*p) * 100 + sizeof (*q);
}

/* A later parameter can use the size of an earlier one.  */

__attribute__ ((noipa)) int
chained_size (n, p, q)
     int n;
     char (*p)[n++];
     char (*q)[sizeof (*p) + n];
{
  n = 100;
  return sizeof (*p) * 100 + sizeof (*q);
}

/* All paths through the body have to see the size computed on entry, not one
   computed at the first use of the type.  */

__attribute__ ((noipa)) int
conditional_size (n, p, t)
     int n;
     char (*p)[n];
     int t;
{
  int r = 0;
  if (t)
    r = sizeof (*p);
  n = 7;
  return r + sizeof (*p);
}

int
main (void)
{
  char b[64];

  global = 0;
  array_static (b, b);
  if (global != 1)
    abort ();

  global = 0;
  array (b);
  array (b);
  if (global != 2)
    abort ();

  global = 0;
  several (b, b, b);
  if (global != 3)
    abort ();

  global = 0;
  two_dimensions (0);
  if (global != 2)
    abort ();

  global = 0;
  pointer_to_vla (0);
  if (global != 1)
    abort ();

  global = 0;
  register_array (b, b);
  if (global != 2)
    abort ();

  atomic_op (b);
  if (atomic_size != 7)
    abort ();

  global = 0;
  compound_literal (b);
  if (global != 1)
    abort ();

  global = 0;
  sizeof_vla (b);
  if (global != 1)
    abort ();

  if (increment (b, 3) != 4)
    abort ();
  if (increment_promoted (b, 3) != 4)
    abort ();
  if (increment_twice (b, b, 3) != 5)
    abort ();
  if (order (b, b, 3) != 9)
    abort ();
  if (before_body (3, b) != 3)
    abort ();
  if (shared_specifier (3, 0, 0) != 4)
    abort ();

  if (pointer_size (3, 0) != 3)
    abort ();
  if (decayed_size (3, 0) != 3)
    abort ();
  if (typeof_size (3, 0) != 3)
    abort ();
  if (struct_size (3, 0) != 3)
    abort ();
  if (shared_size (3, 0, 0) != 306)
    abort ();
  if (chained_size (3, 0, 0) != 307)
    abort ();

  if (conditional_size (3, 0, 0) != 3)
    abort ();
  if (conditional_size (3, 0, 1) != 6)
    abort ();

  return 0;
}
