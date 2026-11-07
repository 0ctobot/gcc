/* PR c/100529 */
/* { dg-do run } */
/* { dg-options "-std=gnu23" } */

/* Since C23, the arms of a conditional can have two struct types of the same
   tag that aren't the same type.  Discarding the value of such a conditional
   used to ICE in fold_convert_loc.  */

extern void abort (void);

static int calls;

__attribute__ ((noipa)) void
first (int c)
{
  struct T { int b; } o;
  o.b = 77;
  (void) (c ? ({ struct T { int b; } t; t.b = 11; calls++; t; }) : o);
}

__attribute__ ((noipa)) void
second (int c)
{
  struct T { int b; } o;
  o.b = 77;
  (void) (c ? o : ({ struct T { int b; } t; t.b = 11; calls++; t; }));
}

int
main (void)
{
  first (0);
  first (1);
  second (0);
  second (1);
  if (calls != 2)
    abort ();
  return 0;
}
