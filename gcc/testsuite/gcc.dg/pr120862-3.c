/* PR c/120862 */
/* { dg-do compile } */
/* { dg-options "-std=gnu23 -fchecking -O0" } */

/* An inlined function owns a variably modified type and calls through a pointer
   to a function returning it.  The local's type is copied, so the function type
   of the copied call has to be copied with it.  */

static inline __attribute__ ((always_inline)) int
f1 (int n, void *v)
{
  struct d { int a[n]; };
  struct d y;
  struct d (*fp) (void) = v;
  y = fp ();
  return y.a[0];
}

int h1 (int m, void *v) { return f1 (m, v); }

/* The type is declared in the prototype.  */

static inline __attribute__ ((always_inline)) int
f2 (int n, struct d { int a[n]; } (*fp) (void))
{
  struct d y;
  y = fp ();
  return y.a[0];
}

int h2 (int m, void *v) { return f2 (m, v); }

static inline __attribute__ ((always_inline)) int
f3 (int n, void *v)
{
  struct d { int a[n]; };
  __typeof__ (struct d (*) (void)) fp = v;
  return fp ().a[0];
}

int h3 (int m, void *v) { return f3 (m, v); }

/* The owner is a nested function inlined into its parent.  */

int
c4 (int k)
{
  __attribute__ ((always_inline)) inline int g (int m, void *v)
  {
    struct s { int a[m]; };
    struct s y;
    y = ((struct s (*) (void)) v) ();
    return y.a[0] + k;
  }
  return g (k, 0) + g (k + 1, 0);
}
