/* PR ipa/128436 */
/* { dg-do compile } */
/* { dg-options "-O3 -Wno-psabi --param=ipa-cp-eval-threshold=1" } */

typedef float v4sf __attribute__ ((vector_size (16)));
typedef int v4si __attribute__ ((vector_size (16)));

union U
{
  v4sf f;
  v4si i;
};

v4si h (v4si);

__attribute__ ((noinline)) v4si
f (union U u)
{
  return h (u.i);
}

v4si
g1 (void)
{
  union U u = { .f = { 0.7f, 0.7f, 0.7f, 0.7f } };
  return f (u);
}

v4si
g2 (void)
{
  union U u = { .f = { 0.7f, 0.7f, 0.7f, 0.7f } };
  return f (u);
}
