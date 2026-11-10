/* { dg-do run } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O2" } */

/* An asm register output to a bit-field of a struct that SRA scalarizes is
   reduced to the width of the field, the same as an output to a bit-field
   in memory.  */

struct u11 { unsigned int a : 5, b : 11, c : 16; };
struct s11 { int a : 5, b : 11, c : 16; };
struct u5 { unsigned char a : 3, b : 5; };
struct u40 { unsigned long a : 24, b : 40; };

__attribute__((noipa)) unsigned long
out_u11 (unsigned long v)
{
  struct u11 x = { 1, 2, 3 };
  __asm__ ("mov %0, %1" : "=r" (x.b) : "r" (v));
  return x.b;
}

__attribute__((noipa)) long
out_s11 (unsigned long v)
{
  struct s11 x = { 1, 2, 3 };
  __asm__ ("mov %0, %1" : "=r" (x.b) : "r" (v));
  return x.b;
}

__attribute__((noipa)) unsigned long
out_u5 (unsigned long v)
{
  struct u5 x = { 1, 2 };
  __asm__ ("mov %0, %1" : "=r" (x.b) : "r" (v));
  return x.b;
}

__attribute__((noipa)) unsigned long
out_u40 (unsigned long v)
{
  struct u40 x = { 1, 2 };
  __asm__ ("mov %0, %1" : "=r" (x.b) : "r" (v));
  return x.b;
}

__attribute__((noipa)) unsigned long
inout_u11 (unsigned long v)
{
  struct u11 x = { 1, 0x7ff, 3 };
  __asm__ ("add %0, %0, %1" : "+r" (x.b) : "r" (v));
  return x.b;
}

__attribute__((noipa)) unsigned long
out_param (unsigned long v, struct u11 x)
{
  __asm__ ("mov %0, %1" : "=r" (x.b) : "r" (v));
  return x.b;
}

int
main (void)
{
  unsigned long v = 0x1122334455667734UL;
  struct u11 x = { 0, 0, 0 };

  if (out_u11 (v) != 0x734)
    __builtin_abort ();
  if (out_s11 (v) != -204)
    __builtin_abort ();
  if (out_u5 (v) != 0x14)
    __builtin_abort ();
  if (out_u40 (v) != 0x4455667734UL)
    __builtin_abort ();
  if (inout_u11 (1) != 0)
    __builtin_abort ();
  if (out_param (v, x) != 0x734)
    __builtin_abort ();
  return 0;
}
