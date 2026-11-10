/* { dg-do run } */
/* { dg-options "-O2" } */

/* A conditional expression that selects the addresses of two members at
   the same offset mustn't be folded to one of them before the object sizes
   have been computed, since the members' sizes differ.  */

typedef __SIZE_TYPE__ size_t;

union addr { char v4[16]; char v6[46]; };
union addr g;

__attribute__ ((noipa)) size_t
tern_g (int sel)
{
  char *p = sel ? g.v6 : g.v4;
  return __builtin_dynamic_object_size (p, 1);
}

__attribute__ ((noipa)) size_t
tern_g_small (int sel)
{
  char *p = sel ? g.v4 : g.v6;
  return __builtin_dynamic_object_size (p, 1);
}

__attribute__ ((noipa)) size_t
tern_loc (int sel)
{
  union addr l;
  char *p = sel ? l.v6 : l.v4;
  size_t r = __builtin_dynamic_object_size (p, 1);
  __asm__ volatile ("" : : "r" (p) : "memory");
  return r;
}

__attribute__ ((noipa)) size_t
tern_loc_small (int sel)
{
  union addr l;
  char *p = sel ? l.v4 : l.v6;
  size_t r = __builtin_dynamic_object_size (p, 1);
  __asm__ volatile ("" : : "r" (p) : "memory");
  return r;
}

__attribute__ ((noipa)) size_t
tern_static (int sel)
{
  char *p = sel ? g.v4 : g.v6;
  return __builtin_object_size (p, 1);
}

int
main (void)
{
  if (tern_g (1) != 46 || tern_g (0) != 16)
    __builtin_abort ();
  if (tern_g_small (1) != 16 || tern_g_small (0) != 46)
    __builtin_abort ();
  if (tern_loc (1) != 46 || tern_loc (0) != 16)
    __builtin_abort ();
  if (tern_loc_small (1) != 16 || tern_loc_small (0) != 46)
    __builtin_abort ();
  if (tern_static (1) != 46)
    __builtin_abort ();
  return 0;
}
