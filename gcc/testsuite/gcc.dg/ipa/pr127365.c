/* { dg-do compile } */
/* { dg-options "-O3" } */

/* Each level of the chain doubles the propagated size cost of the constants
   passed down, so the values of the G and H recursion sum up past INT_MAX.  */

volatile int sink;
int opaque (void);

#define NI __attribute__ ((noinline))
#define LEVEL(n, m)				\
  static NI int f##n (int x)			\
  {						\
    if (x == 1)					\
      sink++;					\
    return f##m (x) + f##m (x ^ 1);		\
  }

static NI int
f30 (int x)
{
  if (x == 1)
    sink++;
  return sink + x;
}

LEVEL (29, 30) LEVEL (28, 29) LEVEL (27, 28) LEVEL (26, 27) LEVEL (25, 26)
LEVEL (24, 25) LEVEL (23, 24) LEVEL (22, 23) LEVEL (21, 22) LEVEL (20, 21)
LEVEL (19, 20) LEVEL (18, 19) LEVEL (17, 18) LEVEL (16, 17) LEVEL (15, 16)
LEVEL (14, 15) LEVEL (13, 14) LEVEL (12, 13) LEVEL (11, 12) LEVEL (10, 11)
LEVEL (9, 10) LEVEL (8, 9) LEVEL (7, 8) LEVEL (6, 7) LEVEL (5, 6)
LEVEL (4, 5) LEVEL (3, 4) LEVEL (2, 3) LEVEL (1, 2) LEVEL (0, 1)

static int h (int);

static NI int
g (int x)
{
  if (sink)
    return h (x) + h (x) + f0 (x);
  return sink + 1;
}

static NI int
h (int x)
{
  if (sink)
    return g (x) + g (x) + f0 (x);
  return sink + 2;
}

int
entry (void)
{
  return g (0) + g (1) + h (0) + h (1) + g (opaque ()) + h (opaque ());
}
