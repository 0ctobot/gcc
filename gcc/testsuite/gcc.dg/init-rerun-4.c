/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-gimple" } */

/* An initializer only runs again within the lifetime of its variable if a jump
   at or below the declaration comes back to a label above it.  A jump wholly
   above the declaration can't do that, and it must not cost the variable a
   temporary as large as itself.  Everything that does run the initializer again
   has to keep that temporary.  */

struct B { char a[2000]; int x; };

extern void use (void *);
extern void two (int, void *);
extern int g (int);
extern int h (void);

/* These don't need a temporary.  */

void
above (void)
{
  int i = 0;
again:
  if (i++ < h ())
    goto again;
  struct B neg = { .x = g (1) };
  use (&neg);
}

void
above_inner (void)
{
  {
    int i = 0;
  again:
    if (i++ < h ())
      goto again;
  }
  struct B neg = { .x = g (1) };
  use (&neg);
}

void
above_asm (void)
{
again:
  asm goto ("" : : : : again);
  struct B neg = { .x = g (1) };
  use (&neg);
}

void
above_literal (void)
{
  int i = 0;
again:
  if (i++ < h ())
    goto again;
  use (&(struct B) { .x = g (1) });
}

/* These do.  */

void
below (void)
{
again:;
  struct B pos = { .x = g (1) };
  use (&pos);
  if (h ())
    goto again;
}

void
in_init (void)
{
again:;
  struct B pos = { .x = ({ if (h ()) goto again; g (1); }) };
  use (&pos);
}

/* The jump to top is above the declaration, but the way to it starts below.  */

void
through_label (void)
{
top:
  g (0);
  goto skip;
mid:
  goto top;
skip:;
  struct B pos = { .x = g (1) };
  use (&pos);
  if (h ())
    goto mid;
}

void
inner_label (void)
{
  {
  again:
    g (0);
  }
  struct B pos = { .x = g (1) };
  use (&pos);
  if (h ())
    goto again;
}

void
below_asm (void)
{
again:;
  struct B pos = { .x = g (1) };
  use (&pos);
  asm goto ("" : : : : again);
}

void
computed (void *l)
{
again:
  if (h ())
    goto *l;
  struct B pos = { .x = g (1) };
  use (&pos);
  l = &&again;
}

void
below_literal (void)
{
again:
  use (&(struct B) { .x = g (1) });
  if (h ())
    goto again;
}

/* The jump can run after the literal, whichever operand goes first.  */

void
literal_operand (void)
{
again:
  two (({ if (h ()) goto again; 1; }), &(struct B) { .x = g (1) });
}

/* { dg-final { scan-tree-dump-not "neg = D\\." "gimple" } } */
/* { dg-final { scan-tree-dump-times "pos = D\\.\[0-9\]+;" 6 "gimple" } } */
/* { dg-final { scan-tree-dump-times "D\\.\[0-9\]+ = D\\.\[0-9\]+;" 2 "gimple" } } */
