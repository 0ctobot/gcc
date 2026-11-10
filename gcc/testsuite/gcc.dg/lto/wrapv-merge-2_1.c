/* { dg-options "-fno-strict-overflow" } */

extern void overflow_handler (void);

int
callee (int x)
{
  /* True for x == INT_MAX, since the addition wraps.  */
  if (x + 1 < 0)
    {
      overflow_handler ();
      return -1;
    }
  return x + 1;
}
