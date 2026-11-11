/* PR middle-end/128302 */
/* __builtin_clear_padding on a union with a _BitInt bit-field member clears
   the bits past the bit-field's width, however wide the _BitInt's limbs
   are.  */
/* { dg-do run { target bitint } } */
/* { dg-options "-std=c23 -O2" } */

union U { unsigned _BitInt(24) f : 20; };
union U u1, u2;
#if __BITINT_MAXWIDTH__ >= 135
union V { unsigned _BitInt(135) f : 130; };
union V v1, v2;
#endif

__attribute__((noipa)) void
foo (union U *u)
{
  u->f = 0xfffff;
}

__attribute__((noipa)) void
clear_u (union U *u)
{
  __builtin_clear_padding (u);
}

#if __BITINT_MAXWIDTH__ >= 135
__attribute__((noipa)) void
bar (union V *v)
{
  v->f = 0x3ffffffffffffffffffffffffffffffffuwb;
}

__attribute__((noipa)) void
clear_v (union V *v)
{
  __builtin_clear_padding (v);
}
#endif

int
main ()
{
  __builtin_memset (&u1, 0, sizeof (u1));
  __builtin_memset (&u2, -1, sizeof (u2));
  foo (&u1);
  clear_u (&u2);
  if (__builtin_memcmp (&u1, &u2, sizeof (u1)))
    __builtin_abort ();
#if __BITINT_MAXWIDTH__ >= 135
  __builtin_memset (&v1, 0, sizeof (v1));
  __builtin_memset (&v2, -1, sizeof (v2));
  bar (&v1);
  clear_v (&v2);
  if (__builtin_memcmp (&v1, &v2, sizeof (v1)))
    __builtin_abort ();
#endif
  return 0;
}
