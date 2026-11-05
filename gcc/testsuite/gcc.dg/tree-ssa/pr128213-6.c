/* PR tree-optimization/128213 */
/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-phiprop1-details" } */

/* The address points 6 bytes into a 9-byte string constant, which only leaves
   3 bytes for the 4-byte load.  */

__INT32_TYPE__ iv;

__INT32_TYPE__
f (int c, int d)
{
  const char *p = c ? "abcdefgh" + 6 : (const char *) &iv;
  if (d)
    {
      __INT32_TYPE__ v;
      __builtin_memcpy (&v, p, sizeof (v));
      return v;
    }
  return 0;
}

/* { dg-final { scan-tree-dump-not "Inserting PHI for result of load" "phiprop1" } } */
