/* { dg-lto-do run } */
/* { dg-lto-options { { -O2 -flto } } } */
/* { dg-require-alias "" } */

/* The prevailing declaration of z is an alias of a body under another name,
   which must stay hidden from a call through a noipa declaration of z.  */

extern void abort (void);

__attribute__ ((noipa)) int z (int);

int
main (void)
{
  int x = z (5);
  if (__builtin_constant_p (x))
    abort ();
  return x != 6;
}
