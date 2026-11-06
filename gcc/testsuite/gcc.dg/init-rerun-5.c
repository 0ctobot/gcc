/* { dg-do compile } */
/* { dg-require-effective-target label_values } */
/* { dg-options "-O2 -fdump-tree-gimple" } */

/* A compound literal in the condition or the increment of a loop gets
   initialized again on every iteration.  Only an initializer that can read the
   old value needs a temporary, and nothing in a loop body or above the loop's
   label gets one from this.  */

struct S { int x; int pad; long a[6]; };

extern void use (struct S *);
extern long g (long);
extern int gi;
extern long gs;
extern volatile int vi;

/* These get a temporary.  */

void
inc (int n)
{
  struct S *p = 0;
  for (int i = 0; i < n; i++, p = &(struct S) { .x = i, .a[3] = g (i) })
    use (p);
}

void
cond (int n)
{
  struct S *p = 0;
  for (int i = 0; p = &(struct S) { .x = i, .a[3] = g (i) }, i < n; i++)
    use (p);
}

void
whl (int n)
{
  struct S *p = 0;
  int i = 0;
  while (p = &(struct S) { .x = i, .a[3] = g (i) }, i++ < n)
    use (p);
}

void
dow (int n)
{
  struct S *p = 0;
  int i = 0;
  do
    use (p);
  while (p = &(struct S) { .x = i, .a[3] = g (i) }, i++ < n);
}

void
read_through_pointer (int n)
{
  struct S *p = &(struct S) { .x = n };
  for (int i = 0; i < n; i++, p = &(struct S) { .x = p->x + 1, .a[3] = n })
    use (p);
}

void
computed (int n)
{
  struct S *p = 0;
  void *l = &&out;
  for (int i = 0; i < n; i++, p = &(struct S) { .x = i, .a[3] = g (i) })
    use (p);
  if (n)
    goto *l;
out:
  use (p);
}

/* Nothing here can read the old value, so these stay in place.  */

void
plain (int n)
{
  struct S *p = 0;
  for (int i = 0; i < n; i++, p = &(struct S) { .x = i, .a[3] = n })
    use (p);
}

void
plain_cond (int n)
{
  struct S *p = 0;
  for (int i = 0; p = &(struct S) { .x = i }, i < n; i++)
    use (p);
}

void
bump (int n)
{
  struct S *p = 0;
  for (int i = 0; i < n; p = &(struct S) { .x = i++, .a[3] = n })
    use (p);
}

void
volatile_read (int n)
{
  struct S *p = 0;
  for (int i = 0; i < n; i++, p = &(struct S) { .x = vi, .a[3] = n })
    use (p);
}

void
store (int n)
{
  struct S *p = 0;
  for (int i = 0; i < n; i++, p = &(struct S) { .x = (gi = i), .a[3] = n })
    use (p);
}

void
no_address (int n)
{
  long s = 0;
  for (int i = 0; i < n; i++, s += ((struct S) { .x = i, .a[3] = g (s) }).a[3])
    ;
  gs = s;
}

void
computed_plain (int n)
{
  struct S *p = 0;
  void *l = &&out;
  for (int i = 0; i < n; i++, p = &(struct S) { .x = i, .a[3] = n })
    use (p);
  if (n)
    goto *l;
out:
  use (p);
}

/* These aren't initialized again within their lifetime.  */

void
in_body (int n)
{
  for (int i = 0; i < n; i++)
    {
      struct S *p = &(struct S) { .x = i, .a[3] = g (i) };
      use (p);
    }
}

void
in_bare_body (int n)
{
  struct S *p = 0;
  for (int i = 0; i < n; i++)
    p = &(struct S) { .x = i, .a[3] = g (p ? p->x : 0) };
  use (p);
}

void
in_first_clause (int n)
{
  struct S *p;
  for (p = &(struct S) { .x = n, .a[3] = g (n) }; n; n--)
    use (p);
}

void
in_statement_expression (int n)
{
  struct S *p = 0;
  for (int i = 0; i < n;
       i++, ({ struct S *q = &(struct S) { .x = i, .a[3] = g (i) }; use (q); }))
    use (p);
}

void
declared_in_first_clause (int n)
{
  for (struct S d0 = { .x = n, .a[3] = g (n) }; n; n--)
    use (&d0);
}

void
declared_in_body (int n)
{
  for (int i = 0; i < n; i++)
    {
      struct S d1 = { .x = i, .a[3] = g (i) };
      use (&d1);
    }
}

void
declared_after (int n)
{
  for (int i = 0; i < n; i++)
    use (0);
  struct S d2 = { .x = n, .a[3] = g (n) };
  use (&d2);
}

void
computed_declared_after (int n)
{
  void *l = &&out;
  for (int i = 0; i < n; i++)
    use (0);
  struct S d3 = { .x = n, .a[3] = g (n) };
  use (&d3);
  if (n)
    goto *l;
out:
  use (0);
}

/* { dg-final { scan-tree-dump-times {D\.[0-9]+ = D\.[0-9]+;} 6 "gimple" } } */
/* { dg-final { scan-tree-dump-not {d[0-9] = D\.} "gimple" } } */
