/* PR c/105180: the size expressions of old-style parameter declarations are
   evaluated on each entry to the function, in that function.  Test nested
   functions, where the size used to be misplaced into the parent function, and
   empty declarations in the parameter declaration list.  */
/* { dg-do run } */
/* { dg-additional-options "-std=gnu17 -w" } */

extern void abort (void);

int global;

__attribute__ ((noipa)) int
func (void)
{
  return ++global;
}

__attribute__ ((noipa)) void
empty_declaration (a)
     int a;
     __typeof__ (char[func ()]);
{
}

__attribute__ ((noipa)) int
empty_struct_declaration (n)
     int n;
     struct s { char x[n++]; };
{
  return n;
}

__attribute__ ((noipa)) void
nested_calls (void)
{
  void bar (s, c)
       char *s;
       char c[static func ()];
  {
  }
  bar ("1", "1");
  bar ("1", "1");
  bar ("1", "1");
}

/* A nested function that's never called mustn't evaluate anything.  */

__attribute__ ((noipa)) void
nested_uncalled (void)
{
  void bar (c)
       char c[func ()];
  {
  }
}

__attribute__ ((noipa)) void
nested_empty_declaration (void)
{
  void bar (a)
       int a;
       __typeof__ (char[func ()]);
  {
  }
  bar (1);
  bar (1);
}

/* The size of the nested function's type is fixed on entry to it and can use a
   variable of the parent.  */

__attribute__ ((noipa)) int
nested_size (m)
     int m;
{
  int bar (p)
      char (*p)[m];
  {
    m = 9;
    return sizeof (*p);
  }
  return bar (0);
}

int
main (void)
{
  global = 0;
  empty_declaration (1);
  if (global != 1)
    abort ();

  if (empty_struct_declaration (3) != 4)
    abort ();

  global = 0;
  nested_calls ();
  if (global != 3)
    abort ();

  global = 0;
  nested_uncalled ();
  if (global != 0)
    abort ();

  global = 0;
  nested_empty_declaration ();
  if (global != 2)
    abort ();

  if (nested_size (4) != 4)
    abort ();

  return 0;
}
