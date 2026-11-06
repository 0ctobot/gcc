/* PR tree-optimization/127874 */
/* { dg-do run } */

/* Redeclaring __builtin_memset as noreturn must not leak into the memset calls
   the compiler generates on its own, such as the one for the loop in k.  */

typedef __SIZE_TYPE__ size_t;

extern void *__builtin_memset (void *, int, size_t) __attribute__((noreturn));
extern void abort (void);

int g;

__attribute__((noipa)) void
f (char *p)
{
  *p = 0;
  if (g)
    __builtin_memset (p, 0, 1);
}

__attribute__((noipa)) void
k (char *p, size_t n)
{
  for (size_t i = 0; i < n; i++)
    p[i] = 0;
}

int
main (void)
{
  static char a[64];

  a[0] = a[63] = 1;
  k (a, 64);
  if (a[0] || a[63])
    abort ();
  f (a);
  return 0;
}
