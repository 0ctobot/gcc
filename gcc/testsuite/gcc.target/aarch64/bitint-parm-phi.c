/* The padding bits of a _BitInt parameter are unspecified on this target, so
   they must be extended even if the parameter only reaches its use through a
   PHI.  The calls through mismatched pointers set those bits.  */
/* { dg-do run { target { bitint && int128 } } } */
/* { dg-options "-O2 -std=gnu23" } */

__attribute__ ((noipa)) long long
phi_s (_BitInt (3) x, int c, _BitInt (3) y)
{
  _BitInt (3) r = c ? x : y;
  return r;
}

__attribute__ ((noipa)) long long
phi_u (unsigned _BitInt (3) x, int c, unsigned _BitInt (3) y)
{
  unsigned _BitInt (3) r = c ? x : y;
  return r;
}

__attribute__ ((noipa)) long long
phi_load (_BitInt (3) x, int c, _BitInt (3) *y)
{
  _BitInt (3) r;
  if (c)
    r = x;
  else
    r = *y;
  return r;
}

__attribute__ ((noipa)) long long
phi_100 (_BitInt (100) x, int c, _BitInt (100) y)
{
  _BitInt (100) r = c ? x : y;
  return r >> 90;
}

int
main (void)
{
  long long (*ps) (int, int, int) = (void *) phi_s;
  long long (*pu) (int, int, int) = (void *) phi_u;
  long long (*pl) (int, int, void *) = (void *) phi_load;
  if (ps (7, 1, 0) != -1 || ps (0, 0, 7) != -1 || ps (0x13, 1, 0) != 3)
    __builtin_abort ();
  if (pu (0xff, 1, 0) != 7 || pu (0, 0, 0xff) != 7)
    __builtin_abort ();
  if (pl (7, 1, 0) != -1)
    __builtin_abort ();
  long long (*p100) (unsigned __int128, int, unsigned __int128)
    = (void *) phi_100;
  unsigned __int128 bits = ((unsigned __int128) 1 << 100) - 1;
  if (p100 (bits, 1, 0) != -1 || p100 (0, 0, bits) != -1)
    __builtin_abort ();
  return 0;
}
