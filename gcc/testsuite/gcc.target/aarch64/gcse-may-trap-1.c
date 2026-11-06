/* Code hoisting must not move a division that can trap above a call that might
   not return, since the division only runs once the call has come back.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-rtl-hoist-details" } */

extern volatile int vd;
extern void maybe_exit (void);
extern void g (int);

__attribute__ ((cold)) int
f (int c, int x, int y)
{
  int r;
  if (c)
    goto a;
  maybe_exit ();
  if (vd)
    {
      r = x / y;
      g (r);
      return r - 1;
    }
a:
  r = x / y;
  g (r);
  return r * 3;
}

/* { dg-final { scan-rtl-dump-not "copying expression" "hoist" } } */
