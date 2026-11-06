/* PR tree-optimization/126197 */
/* { dg-do compile } */
/* { dg-additional-options "-fexceptions -fnon-call-exceptions" } */

/* VRP prunes cases from the switch in the cleanup handler in the same run that
   folds the divisor to a constant, which stops the division from trapping.
   Purging the EH edge then deletes the handler with the pruned case edges.  */

int g, h;

static inline __attribute__ ((always_inline)) void
cl (int *p)
{
  switch (*p)
    {
    case 0: g = 11; break;
    case 1: g = 22; break;
    case 2: g = 33; break;
    case 3: g = 44; break;
    case 8: g = 55; break;
    case 16: g = 66; break;
    case 32: g = 88; break;
    default: g = 77; break;
    }
}

int
f (int x, unsigned long n)
{
  if (n < 3)
    return 0;
  if (n > 3)
    return 0;
  {
    int r __attribute__ ((cleanup (cl))) = x & 3;
    h = x;
    h = x / n;
  }
  return x;
}
