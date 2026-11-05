/* PR c/120862 */
/* { dg-do compile } */
/* { dg-options "-std=gnu23 -fchecking -O2" } */

/* Same as pr120862-3.c for functions inlined by the optimizers.  */

static int
f1 (int n, void *v)
{
  struct d { int a[n]; };
  int r = 0;
  {
    struct d y;
    y = ((struct d (*) (void)) v) ();
    r = y.a[0];
  }
  return r;
}

int h1 (int m, void *v) { return f1 (m, v); }

/* The returned type has a constant size and is only variably modified through
   a member.  */

static inline __attribute__ ((always_inline)) int
f2 (int n, void *v)
{
  struct d { int (*p)[n]; };
  struct d y;
  y = ((struct d (*) (void)) v) ();
  return y.p != 0;
}

int h2 (int m, void *v) { return f2 (m, v); }

/* Two levels of inlining, with both copies of the function.  */

static inline int
f3 (int n, void *v)
{
  struct d { int a[n]; };
  struct d y;
  y = ((struct d (*) (void)) v) ();
  return y.a[0];
}

static inline int f4 (int n, void *v) { return f3 (n, v) + 1; }

int h3 (int m, void *v) { return f4 (m, v) + f4 (m + 1, v); }
