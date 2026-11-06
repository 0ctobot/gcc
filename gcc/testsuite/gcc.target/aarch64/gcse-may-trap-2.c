/* RTL PRE must not move a division that can trap above a call that might not
   return by computing it on the path that didn't compute it yet.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-rtl-pre-details" } */

extern void maybe_exit (void);
extern void g (int);

int
f (int c, int x, int y)
{
  int r = 0;
  if (c)
    {
      r = x / y;
      g (r);
    }
  maybe_exit ();
  return r + x / y;
}

/* { dg-final { scan-rtl-dump-not "PRE: edge" "pre" } } */
