/* PR c/100529 */
/* { dg-do run } */
/* { dg-options "-std=gnu23" } */

/* Since C23, the arms of a conditional can have two variably sized struct
   types of the same tag.  The size of the result has to come from the arm
   that the condition selects, since an arm's type can be declared inside of
   that arm and then has no size when the other arm runs.  */

extern void abort (void);

static int calls;

__attribute__ ((noipa)) int
gn (void)
{
  calls++;
  return 8;
}

#define MK(v) \
  ({ int m = gn (); struct T { char a[m]; } t; t.a[0] = (v); t.a[7] = (v) + 10; t; })

#define OUTER \
  int n = gn (); \
  struct T { char a[n]; } o; \
  o.a[0] = 1; \
  o.a[7] = 11

__attribute__ ((noipa)) int
first (int c)
{
  OUTER;
  return (c ? MK (2) : o).a[7];
}

__attribute__ ((noipa)) int
second (int c)
{
  OUTER;
  return (c ? o : MK (2)).a[7];
}

__attribute__ ((noipa)) int
comma (int c)
{
  OUTER;
  return (c ? ((void) gn (), MK (2)) : o).a[7];
}

__attribute__ ((noipa)) int
nested (int c)
{
  OUTER;
  return (c ? (c > 1 ? MK (2) : o) : o).a[7];
}

__attribute__ ((noipa)) int
member (int c)
{
  OUTER;
  return (c ? ({ int m = gn ();
		 struct U { struct T { char a[m]; } s; } u;
		 u.s.a[7] = 12;
		 u; }).s : o).a[7];
}

__attribute__ ((noipa)) int
auto_result (int c)
{
  OUTER;
  __auto_type r = c ? MK (2) : o;
  return r.a[0] + r.a[7];
}

/* The size of neither arm's type depends on the arm, since both were evaluated
   at the declarations of x and y.  */

__attribute__ ((noipa)) int
auto_both (int c)
{
  __auto_type x = MK (2);
  __auto_type y = MK (3);
  return (c ? x : y).a[7];
}

/* An arm that isn't selected must not run the call in the size of its type.  */

__attribute__ ((noipa)) int
call_size (int c)
{
  OUTER;
  calls = 0;
  int r = (c ? ({ struct T { char a[gn ()]; } t; t.a[7] = 12; t; }) : o).a[7];
  if (calls != !!c)
    abort ();
  return r;
}

int
main (void)
{
  if (first (1) != 12 || first (0) != 11)
    abort ();
  if (second (1) != 11 || second (0) != 12)
    abort ();
  if (comma (1) != 12 || comma (0) != 11)
    abort ();
  if (nested (2) != 12 || nested (1) != 11 || nested (0) != 11)
    abort ();
  if (member (1) != 12 || member (0) != 11)
    abort ();
  if (auto_result (1) != 14 || auto_result (0) != 12)
    abort ();
  if (auto_both (1) != 12 || auto_both (0) != 13)
    abort ();
  if (call_size (1) != 12 || call_size (0) != 11)
    abort ();
  return 0;
}
