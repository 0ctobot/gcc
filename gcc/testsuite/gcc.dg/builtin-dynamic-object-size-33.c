/* { dg-do run } */
/* { dg-options "-O2 -fdump-ipa-cp-details" } */

/* IPA-CP passes the address of a member of a known object to a function as
   &MEM[&obj + 4B], which no longer names the member.  The minimum size of
   the subobject must still stay within the member, the same as for pointer
   arithmetic on a member, which folding turns into the same form.  The
   maximum may be what's left in the whole object.  */

typedef __SIZE_TYPE__ size_t;

struct T { char pad[4]; union { char small[4]; char big[16]; } u; };
static struct T obj;

#define NI static __attribute__ ((noinline))

NI size_t dsz3 (char *p) { return __builtin_dynamic_object_size (p, 3); }
NI size_t ssz3 (char *p) { return __builtin_object_size (p, 3); }
NI size_t dsz1 (char *p) { return __builtin_dynamic_object_size (p, 1); }
NI size_t ssz1 (char *p) { return __builtin_object_size (p, 1); }

NI size_t via_dsz3 (struct T *p) { return dsz3 (p->u.small); }
NI size_t via_ssz3 (struct T *p) { return ssz3 (p->u.small); }
NI size_t via_dsz1 (struct T *p) { return dsz1 (p->u.small); }
NI size_t via_ssz1 (struct T *p) { return ssz1 (p->u.small); }

NI size_t
arith_dsz3 (void)
{
  char *q = obj.u.small;
  q += 2;
  return __builtin_dynamic_object_size (q, 3);
}

NI size_t
arith_ssz3 (void)
{
  char *q = obj.u.small;
  q += 2;
  return __builtin_object_size (q, 3);
}

int
main (void)
{
  if (via_dsz3 (&obj) > sizeof (obj.u.small)
      || via_ssz3 (&obj) > sizeof (obj.u.small))
    __builtin_abort ();
  if (via_dsz1 (&obj) < sizeof (obj.u.small)
      || via_ssz1 (&obj) < sizeof (obj.u.small))
    __builtin_abort ();
  if (arith_dsz3 () > sizeof (obj.u.small) - 2
      || arith_ssz3 () > sizeof (obj.u.small) - 2)
    __builtin_abort ();
  return 0;
}

/* { dg-final { scan-ipa-dump-times "replacing param #0 p with const &MEM" 4 "cp" } } */
