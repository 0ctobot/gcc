/* { dg-do run } */
/* { dg-additional-options "-fno-strict-aliasing" } */

/* Conditional store replacement used to load the old contents of memory with
   the type of the store it was moving.  When that type is _Bool the load can
   see a byte which isn't a valid _Bool, and everything after the PHI is free
   to assume the value is 0 or 1.  */

union u0 { unsigned char f1; _Bool f2; };
union u1 { unsigned char a[4]; _Bool b[4]; };
struct s0 { _Bool flag; char pad; };

unsigned int g1;
void *g13;
union u0 g18;
_Bool g_ld;
union u0 g_mem;
union u1 g_arr;
struct s0 g_str;

__attribute__((noipa)) int
test_goto (void)
{
  _Bool c12;
  unsigned long long v15 = 0;

  c12 = g1;
lbl:
  if (c12)
    g_ld = *(_Bool *) g13;
  g18.f1 = 45;
  c12 = v15;
  g18 = c12 ? g18 : (union u0) { .f2 = 1 };
  if (g18.f1 == 45)
    return 0;
  v15 = 102276357045975979;
  goto lbl;
}

__attribute__((noipa)) int
test_const (int c)
{
  g_mem.f1 = 45;
  if (c)
    g_mem.f2 = 1;
  return g_mem.f1 == 45;
}

__attribute__((noipa)) int
test_ssa (int c, _Bool v)
{
  g_mem.f1 = 45;
  if (c)
    g_mem.f2 = v;
  return g_mem.f1 == 45;
}

__attribute__((noipa)) int
test_ptr (void *p, int c)
{
  *(unsigned char *) p = 45;
  if (c)
    *(_Bool *) p = 1;
  return *(unsigned char *) p == 45;
}

__attribute__((noipa)) int
test_array (int i, int c)
{
  g_arr.a[i] = 45;
  if (c)
    g_arr.b[i] = 1;
  return g_arr.a[i] == 45;
}

/* A local union reaches the store sinking without a dominating store being
   needed, provided it isn't scalarized first.  */
__attribute__((noipa, optimize ("no-tree-sra"))) int
test_local (int c)
{
  union u0 l;

  l.f1 = 45;
  if (c)
    l.f2 = 1;
  return l.f1 == 45;
}

/* Without a dominating store the old byte comes from an actual load, which has
   to reach the store unchanged.  */
__attribute__((noipa, optimize ("allow-store-data-races"))) void
test_load (int c)
{
  if (c)
    g_mem.f2 = 1;
}

/* A plain _Bool flag has to keep working, including the read of it as _Bool
   after the store has been sunk.  */
__attribute__((noipa)) int
test_flag (struct s0 *p, int c)
{
  p->flag = 0;
  if (c)
    p->flag = 1;
  return p->flag + 10;
}

#ifdef __BITINT_MAXWIDTH__
/* A _BitInt narrower than its mode gets masked or extended when it's loaded,
   which must not reach the padding bits of the bytes being stored back.  */
union u2 { unsigned short s; unsigned _BitInt(13) b; };
union u2 g_bi;

__attribute__((noipa)) unsigned short
test_bitint (int c)
{
  g_bi.s = 0xe123;
  if (c)
    g_bi.b = 1;
  return g_bi.s;
}

__attribute__((noipa, optimize ("allow-store-data-races"))) void
test_bitint_load (int c)
{
  if (c)
    g_bi.b = 1;
}
#endif

/* An unknown store between the dominating store and the store being sunk keeps
   value numbering from forwarding a value, which leaves an actual load as the
   PHI's argument.  Nothing may assume that load holds 0 or 1.  */
__attribute__((noipa)) int
test_alias (unsigned char *q, unsigned char v, int c)
{
  g_mem.f1 = 0;
  *q = v;
  if (c)
    g_mem.f2 = 1;
  return g_mem.f1 == 45;
}

int
main (void)
{
  unsigned char byte;

  if (test_goto () != 0)
    __builtin_abort ();

  if (test_const (0) != 1 || g_mem.f1 != 45)
    __builtin_abort ();
  if (test_const (1) != 0 || g_mem.f1 != 1)
    __builtin_abort ();

  if (test_ssa (0, 1) != 1 || g_mem.f1 != 45)
    __builtin_abort ();
  if (test_ssa (1, 0) != 0 || g_mem.f1 != 0)
    __builtin_abort ();
  if (test_ssa (1, 1) != 0 || g_mem.f1 != 1)
    __builtin_abort ();

  if (test_ptr (&byte, 0) != 1 || byte != 45)
    __builtin_abort ();
  if (test_ptr (&byte, 1) != 0 || byte != 1)
    __builtin_abort ();

  if (test_array (2, 0) != 1 || g_arr.a[2] != 45)
    __builtin_abort ();
  if (test_array (3, 1) != 0 || g_arr.a[3] != 1)
    __builtin_abort ();

  if (test_local (0) != 1 || test_local (1) != 0)
    __builtin_abort ();

  g_mem.f1 = 45;
  test_load (0);
  if (g_mem.f1 != 45)
    __builtin_abort ();
  test_load (1);
  if (g_mem.f1 != 1)
    __builtin_abort ();

  if (test_flag (&g_str, 0) != 10 || g_str.flag != 0)
    __builtin_abort ();
  if (test_flag (&g_str, 1) != 11 || g_str.flag != 1)
    __builtin_abort ();

#ifdef __BITINT_MAXWIDTH__
  if (sizeof (unsigned _BitInt(13)) == sizeof (unsigned short))
    {
      if (test_bitint (0) != 0xe123)
	__builtin_abort ();
      if (test_bitint (1) != 1)
	__builtin_abort ();
      g_bi.s = 0xe123;
      test_bitint_load (0);
      if (g_bi.s != 0xe123)
	__builtin_abort ();
      test_bitint_load (1);
      if (g_bi.s != 1)
	__builtin_abort ();
    }
#endif

  if (test_alias (&g_mem.f1, 45, 0) != 1 || g_mem.f1 != 45)
    __builtin_abort ();
  if (test_alias (&g_mem.f1, 45, 1) != 0 || g_mem.f1 != 1)
    __builtin_abort ();

  return 0;
}
