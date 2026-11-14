/* { dg-lto-do run } */
/* { dg-require-weak "" } */
/* { dg-require-alias "" } */
/* { dg-lto-options { { -O2 -flto } { -O0 -flto } { -O2 -flto -flto-partition=max } } } */

/* An alias of a weak definition names the storage of that definition, even
   when a definition in another LTO object overrides it.  This lets the
   overriding function fall back to the weak one through the alias.  */

struct p { int x, y; };

__attribute__ ((weak)) const struct p w = { 1, 2 };
extern const struct p a __attribute__ ((alias ("w")));

__attribute__ ((weak)) int
f (int x)
{
  return x + 1;
}

int f_base (int) __attribute__ ((alias ("f")));

extern const struct p *get_w (void);

__attribute__ ((noipa)) const struct p *
get_a (void)
{
  return &a;
}

__attribute__ ((noipa)) int
sum_a (void)
{
  struct p copy = a;
  return copy.x + copy.y;
}

int
main (void)
{
  if (sum_a () != 3 || get_a ()->x != 1 || get_w ()->x != 70)
    __builtin_abort ();
  if (get_a () == get_w ())
    __builtin_abort ();
  if (f (1) != 11 || f_base (1) != 2)
    __builtin_abort ();
  return 0;
}
