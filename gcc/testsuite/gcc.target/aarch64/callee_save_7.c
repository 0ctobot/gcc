/* { dg-do compile } */
/* { dg-options "-O2 -march=armv8-a+lse" } */

/* Reduced from atomic-inst-ldadd.c.  Parts of the __int128 values are live
   across the call to __atomic_fetch_add_16 and are copied from argument
   registers, so a copy preference lowers the cost of those registers by the
   cost of a move.  That can make an argument register cost the same as a
   callee-saved register even though the bias toward callee-saved registers was
   applied.  Such a tie should be left alone: x19 and x20 would need a move as
   well as a save and a restore, while the argument registers are stored around
   the call in one STP and loaded in one LDP.  */

__int128
f (__int128 *val, __int128 foo)
{
  return __atomic_add_fetch (val, foo, __ATOMIC_RELAXED);
}

__int128
g (__int128 *val, long v)
{
  __int128 foo = v;
  return __atomic_fetch_add (val, foo, __ATOMIC_RELAXED) + foo;
}

/* { dg-final { scan-assembler-not {\m[xw]19\M} } } */
/* { dg-final { scan-assembler-not {\m[xw]20\M} } } */
