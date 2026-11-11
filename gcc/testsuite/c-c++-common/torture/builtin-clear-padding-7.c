/* PR middle-end/128302 */
/* { dg-do run } */
/* { dg-require-effective-target int32plus } */

/* The bits of a union past the width of a bit-field member are padding
   unless another member covers them.  */

union A { unsigned int f : 20; };
union B { unsigned int f : 20; unsigned int g; };
union C { unsigned int f : 20; unsigned char c; };
union D { unsigned int a : 3; unsigned int b : 12; };
struct E { char a; union A u; char b; };
union F { struct { char x; union A u; } s; unsigned char c; };

union A a1, a2;
union B b1, b2;
union C c1, c2;
union D d1, d2;
struct E e1, e2;
union F f1, f2;

__attribute__((noipa)) void
foo (union A *a, union B *b, union C *c, union D *d, struct E *e,
     union F *f)
{
  a->f = 0xfffff;
  b->g = -1;
  c->f = 0xfffff;
  c->c = 0xff;
  d->a = 0x7;
  d->b = 0xfff;
  e->a = -1;
  e->u.f = 0xfffff;
  e->b = -1;
  f->s.x = -1;
  f->s.u.f = 0xfffff;
  f->c = 0xff;
}

int
main ()
{
  __builtin_memset (&a1, 0, sizeof (a1));
  __builtin_memset (&b1, 0, sizeof (b1));
  __builtin_memset (&c1, 0, sizeof (c1));
  __builtin_memset (&d1, 0, sizeof (d1));
  __builtin_memset (&e1, 0, sizeof (e1));
  __builtin_memset (&f1, 0, sizeof (f1));
  __builtin_memset (&a2, -1, sizeof (a2));
  __builtin_memset (&b2, -1, sizeof (b2));
  __builtin_memset (&c2, -1, sizeof (c2));
  __builtin_memset (&d2, -1, sizeof (d2));
  __builtin_memset (&e2, -1, sizeof (e2));
  __builtin_memset (&f2, -1, sizeof (f2));
  foo (&a1, &b1, &c1, &d1, &e1, &f1);
  foo (&a2, &b2, &c2, &d2, &e2, &f2);
  __builtin_clear_padding (&a2);
  __builtin_clear_padding (&b2);
  __builtin_clear_padding (&c2);
  __builtin_clear_padding (&d2);
  __builtin_clear_padding (&e2);
  __builtin_clear_padding (&f2);
  if (__builtin_memcmp (&a1, &a2, sizeof (a1))
      || __builtin_memcmp (&b1, &b2, sizeof (b1))
      || __builtin_memcmp (&c1, &c2, sizeof (c1))
      || __builtin_memcmp (&d1, &d2, sizeof (d1))
      || __builtin_memcmp (&e1, &e2, sizeof (e1))
      || __builtin_memcmp (&f1, &f2, sizeof (f1)))
    __builtin_abort ();
  return 0;
}
