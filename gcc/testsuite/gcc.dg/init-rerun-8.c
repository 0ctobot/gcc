/* { dg-do compile } */
/* { dg-options "-O2 -ftrivial-auto-var-init=zero -fdump-tree-gimple" } */

/* The jump into a loop's condition skips the compound literal in its
   increment, but nothing can store to the literal before it's reached, so the
   first run of its initializer still gets the automatic initialization.  */

struct S { int x; int pad; long a[6]; };

extern void use (struct S *);
extern long g (long);

void
inc (int n)
{
  struct S *p = 0;
  for (int i = 0; i < n; i++, p = &(struct S) { .x = i, .a[3] = g (i) })
    use (p);
}

/* { dg-final { scan-tree-dump-times {\.DEFERRED_INIT} 1 "gimple" } } */
