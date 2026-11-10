/* { dg-do run } */
/* { dg-options "-O2" } */

/* IPA-CP mustn't take the addresses of two members at the same offset for
   one constant argument, since the members' object sizes differ.  Without
   a clone for each address, the size of the parameter isn't known.  */

typedef __SIZE_TYPE__ size_t;

union addr { char v4[16]; char v6[46]; };
union addr g;

static __attribute__ ((noinline)) size_t
sz (char *p)
{
  return __builtin_dynamic_object_size (p, 1);
}

__attribute__ ((noipa)) size_t
call_v4 (void)
{
  return sz (g.v4);
}

__attribute__ ((noipa)) size_t
call_v6 (void)
{
  return sz (g.v6);
}

int
main (void)
{
  size_t s4 = call_v4 (), s6 = call_v6 ();

  if ((s4 != 16 && s4 != (size_t) -1) || (s6 != 46 && s6 != (size_t) -1))
    __builtin_abort ();
  return 0;
}
