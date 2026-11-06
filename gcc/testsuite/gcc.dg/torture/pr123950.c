/* { dg-do run } */
/* { dg-require-effective-target int128 } */
/* { dg-additional-options "-fsigned-char -Wno-psabi" } */

/* WA is passed by reference and copied to a local stack slot on entry.  The
   read of the modified local copy must not be merged with a read of the
   caller's copy.  */

typedef __attribute__((__vector_size__(64))) char V;
typedef __attribute__((__vector_size__(64))) __int128 W;

__attribute__((__noipa__)) void
foo (char, char, char, char, char, char, char, __int128 a, __int128 b,
     char c, char d, char e, long f, long g,
     V va, V vb, char h, char i,
     char j, char k, char l, V vc,
     char m, W wa, char *ret)
{
  wa |= 2;
  __builtin_memmove (&wa, &vc, 26);
  wa ^= 100;
  V vr = (V) wa + j + k + l + m;
  V ur = vb + vr[25] + h + i + va;
  __int128 z = e + d + c + a + b + ur[4] + f + g;
  *ret = z;
}

int
main (void)
{
  char x;
  foo (0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       (V) {}, (V) {}, 0, 0, 0, 0,
       0, (V) {}, 0, (W) {0, -100}, &x);
  if (x)
    __builtin_abort ();
  return 0;
}
