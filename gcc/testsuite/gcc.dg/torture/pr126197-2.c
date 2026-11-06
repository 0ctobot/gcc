/* PR tree-optimization/126197 */
/* { dg-do compile } */
/* { dg-additional-options "-fexceptions -fnon-call-exceptions" } */

/* Final VRP folds the divisor to a constant, which stops the division from
   trapping, and purging its EH edge deletes the handler along with the
   condition that has a __builtin_unreachable arm.  */

int g, h;
void use (char *);

static inline void
cl (int *p)
{
  if (*p == 7)
    __builtin_unreachable ();
  g = *p;
}

int
f (int x)
{
  char buf[8];
  __builtin_strcpy (buf, "abc");
  use (buf);
  __builtin_strcpy (buf, "abc");
  {
    int r __attribute__ ((cleanup (cl))) = x;
    h = x;
    h = x / __builtin_strlen (buf);
  }
  return x;
}
