/* PR middle-end/121106 */
/* The parity insns are only available without POPCNT, so a .PARITY call
   created for the default target can't use them after it's been inlined
   into a function using POPCNT or cloned for POPCNT.  */
/* { dg-do compile } */
/* { dg-options "-O2 -mno-popcnt" } */
/* { dg-require-ifunc "" } */

static int
f (unsigned short x)
{
  return __builtin_parityll (x);
}

__attribute__((target ("popcnt"))) int
g (unsigned short x)
{
  return f (x) + 1;
}

__attribute__((target_clones ("default", "popcnt"))) int
h (unsigned short x)
{
  return __builtin_parityll (x);
}

/* { dg-final { scan-assembler {\tpopcnt[lw]?\t} } } */
