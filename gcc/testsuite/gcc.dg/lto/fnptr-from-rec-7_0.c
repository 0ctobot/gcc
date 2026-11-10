/* { dg-lto-do link } */
/* { dg-lto-options { { -O2 -flto -fdump-ipa-devirt } } } */

/* A const table stores fa to field f, and another unit stores fb to it
   through a pointer to the unqualified record.  Both stores have to be
   noted for the same field, so calls through a pointer to the const
   record must not be speculated to fa.  Field g only ever holds fa, so
   calls through it are still speculated.  */

struct ops
{
  long pad;
  int (*f) (int);
  int (*g) (int);
};

int __attribute__ ((noinline)) fa (int x) { return x + 1; }

const struct ops cops = { 0, fa, fa };

void set_fb (struct ops *);

/* Externally visible, so WPA doesn't treat these as called once from main
   and therefore cold, which would suppress every speculation.  */

int __attribute__ ((noinline, noclone, externally_visible))
call_f (const struct ops *o, int x)
{
  return o->f (x);
}

int __attribute__ ((noinline, noclone, externally_visible))
call_g (const struct ops *o, int x)
{
  return o->g (x);
}

int
main (void)
{
  static struct ops l;
  set_fb (&l);
  return call_f (&l, 1) + call_f (&cops, 1) + call_g (&cops, 1) != 105;
}

/* { dg-final { scan-wpa-ipa-dump-not "speculative call call_f/" "devirt" } } */
/* { dg-final { scan-wpa-ipa-dump "speculative call call_g/\[0-9\]+ => fa/" "devirt" } } */
