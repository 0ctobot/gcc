/* { dg-lto-do link } */
/* { dg-lto-options { { -O2 -flto -fdump-ipa-devirt } } } */

/* The static initializer of gops is the only place that stores a function's
   address to field f, while another unit stores whatever function a
   parameter holds, which is fb.  Calls through f must not be speculated to
   fa whichever order WPA reads the units in.  Here the static initializer
   is read before the store through the parameter; fnptr-from-rec-6 covers
   the other order.  Field g only ever holds fa, so calls through it are
   still speculated.  */

struct ops
{
  long pad;
  int (*f) (int);
  int (*g) (int);
};

extern struct ops gops;
int fb (int);
void set_f (struct ops *, int (*) (int));

/* Externally visible, so WPA doesn't treat these as called once from main
   and therefore cold, which would suppress every speculation.  */

int __attribute__ ((noinline, noclone, externally_visible))
call_f (struct ops *o, int x)
{
  return o->f (x);
}

int __attribute__ ((noinline, noclone, externally_visible))
call_g (struct ops *o, int x)
{
  return o->g (x);
}

int
main (void)
{
  static struct ops l;
  set_f (&l, fb);
  return call_f (&l, 1) + call_f (&gops, 1) + call_g (&gops, 1) != 105;
}

/* { dg-final { scan-wpa-ipa-dump-not "speculative call call_f/" "devirt" } } */
/* { dg-final { scan-wpa-ipa-dump "speculative call call_g/\[0-9\]+ => fa/" "devirt" } } */
