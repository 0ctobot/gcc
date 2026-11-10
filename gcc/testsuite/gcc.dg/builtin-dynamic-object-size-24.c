/* { dg-do run } */
/* { dg-options "-O2" } */

/* Two PHIs that select the addresses of two members at the same offset in
   opposite orders mustn't be merged before the object sizes have been
   computed, since the members' sizes differ.  */

typedef __SIZE_TYPE__ size_t;

union addr { char v4[16]; char v6[46]; };
union addr g;

__attribute__ ((noipa)) size_t
two_phi (int sel, size_t *qs)
{
  char *p, *q;

  if (sel)
    p = g.v4;
  else
    p = g.v6;
  __asm__ volatile ("" ::: "memory");
  if (sel)
    q = g.v6;
  else
    q = g.v4;
  *qs = __builtin_dynamic_object_size (q, 1);
  return __builtin_dynamic_object_size (p, 1);
}

__attribute__ ((noipa)) size_t
two_phi_static (int sel, size_t *qs)
{
  char *p, *q;

  if (sel)
    p = g.v4;
  else
    p = g.v6;
  __asm__ volatile ("" ::: "memory");
  if (sel)
    q = g.v6;
  else
    q = g.v4;
  *qs = __builtin_object_size (q, 1);
  return __builtin_object_size (p, 1);
}

int
main (void)
{
  size_t qs;

  if (two_phi (1, &qs) != 16 || qs != 46)
    __builtin_abort ();
  if (two_phi (0, &qs) != 46 || qs != 16)
    __builtin_abort ();
  if (two_phi_static (1, &qs) != 46 || qs != 46)
    __builtin_abort ();
  return 0;
}
