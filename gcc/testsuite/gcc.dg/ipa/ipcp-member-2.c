/* { dg-do run } */
/* { dg-options "-O2" } */

/* IPA-CP mustn't pass the address of a union member to a function that
   converts it back to the union and takes the address of a larger member,
   since the address of the smaller member gives the larger one a smaller
   object size.  */

typedef __SIZE_TYPE__ size_t;

union u { char small[4]; char big[16]; };
static union u obj;

static __attribute__ ((noinline)) size_t
sz (char *p)
{
  return __builtin_dynamic_object_size (p, 1);
}

static __attribute__ ((noinline)) size_t
via (char (*p)[4])
{
  return sz (((union u *) p)->big);
}

int
main (void)
{
  size_t s = via (&obj.small);

  if (s != 16 && s != (size_t) -1)
    __builtin_abort ();
  return 0;
}
