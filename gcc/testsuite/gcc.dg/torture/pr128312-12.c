/* PR middle-end/128312 */
/* { dg-do run } */
/* { dg-require-effective-target label_values } */
/* { dg-require-effective-target nonlocal_goto } */

/* An iteration statement is a block, so a compound literal in the condition or
   the increment of a loop is one object that gets initialized again on every
   iteration, even though no user label comes back to it.  The initializers here
   read the old value through a pointer and must see it intact until the new one
   is complete.  */

extern void abort (void);
extern __SIZE_TYPE__ strlen (const char *);

struct S { int x; int pad; long a[6]; };
struct C { char s[8]; long n; };

static int seen;

__attribute__ ((noipa)) long
g (long x)
{
  return x;
}

/* The nth value has x = 5 + n and holds the x of the value before it.  */
__attribute__ ((noipa)) void
check (struct S *p)
{
  if (p->x != 5 + seen || p->a[3] != (seen ? 4 + seen : -1))
    abort ();
  seen++;
}

__attribute__ ((noipa)) void
done (int n)
{
  if (seen != n)
    abort ();
  seen = 0;
}

#define NEXT(p) { .x = (p) ? (p)->x + 1 : 5, .a[3] = g ((p) ? (p)->x : -1) }

__attribute__ ((noipa)) void
in_increment (void)
{
  struct S *p = 0;
  for (int i = 0; i < 3; i++, p = &(struct S) NEXT (p))
    if (p)
      check (p);
  done (2);
}

__attribute__ ((noipa)) void
in_condition (void)
{
  struct S *p = 0;
  for (int i = 0; p = &(struct S) NEXT (p), i < 2; i++)
    check (p);
  done (2);
}

__attribute__ ((noipa)) void
in_while (void)
{
  struct S *p = 0;
  int i = 0;
  while (p = &(struct S) NEXT (p), i++ < 2)
    check (p);
  done (2);
}

__attribute__ ((noipa)) void
in_do_while (void)
{
  struct S *p = 0;
  int i = 0;
  do
    if (p)
      check (p);
  while (p = &(struct S) NEXT (p), i++ < 2);
  done (2);
}

__attribute__ ((noipa)) void
in_inner_loop (void)
{
  struct S *p = 0;
  for (int j = 0; j < 1; j++)
    for (int i = 0; i < 3; i++, p = &(struct S) NEXT (p))
      if (p)
	check (p);
  done (2);
}

__attribute__ ((noipa)) void
around_inner_loop (void)
{
  struct S *p = 0;
  for (int i = 0; i < 3; i++, p = &(struct S) NEXT (p))
    for (int j = 0; j < 2; j++)
      if (p && !j)
	check (p);
  done (2);
}

__attribute__ ((noipa)) void
in_statement_expression (void)
{
  struct S *p = 0;
  int r = ({
    for (int i = 0; i < 3; i++, p = &(struct S) NEXT (p))
      if (p)
	check (p);
    1;
  });
  if (r != 1)
    abort ();
  done (2);
}

__attribute__ ((noipa)) void
in_clause_of_outer_loop (void)
{
  struct S *p = 0;
  for (int j = 0; j < 1; j++, ({
	 for (int i = 0; i < 3; i++, p = &(struct S) NEXT (p))
	   if (p)
	     check (p);
       }))
    ;
  done (2);
}

/* Every user label has to count with a computed goto, but the loop's artificial
   label still goes by where its jump is.  */
__attribute__ ((noipa)) void
with_computed_goto (int n)
{
  struct S *p = 0;
  void *l = &&out;
  for (int i = 0; i < 3; i++, p = &(struct S) NEXT (p))
    if (p)
      check (p);
  if (n)
    goto *l;
out:
  done (2);
}

/* Likewise with a nested function that can jump to a label here.  */
__attribute__ ((noipa)) void
with_nested_function (void)
{
  __label__ out;
  struct S *p = 0;
  void h (void) { goto out; }
  for (int i = 0; i < 3; i++, p = &(struct S) NEXT (p))
    if (p)
      check (p);
  if (!p)
    h ();
out:
  done (2);
}

/* The builtin has no side effects to show that it reads the old string, which
   the second run shortens before it gets to the call.  */
__attribute__ ((noipa)) void
through_strlen (void)
{
  const char *q = "abc";
  struct C *p = 0;
  for (int i = 0; i < 3;
       i++, q = p ? p->s : q,
       p = &(struct C) { .s = { 'a', i == 1 ? 'b' : 0 }, .n = strlen (q) })
    if (p && p->n != (i == 1 ? 3 : 2))
      abort ();
}

int
main (void)
{
  in_increment ();
  in_condition ();
  in_while ();
  in_do_while ();
  in_inner_loop ();
  around_inner_loop ();
  in_statement_expression ();
  in_clause_of_outer_loop ();
  with_computed_goto (0);
  with_nested_function ();
  through_strlen ();
  return 0;
}
