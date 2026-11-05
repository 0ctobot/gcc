/* PR ipa/121112 */
/* { dg-do run } */
/* { dg-require-ifunc "" } */

/* An alias of an ifunc symbol names the ifunc symbol, not its resolver, which
   means a call through either one runs whatever function the resolver picked.
   Nothing known about the resolver describes such a call, so it cannot be
   inlined, cloned, deleted or have its side effects ignored.  The resolver
   itself stays an ordinary function that can be called and optimized.  */

typedef void fn_t (void);
typedef void arg_fn_t (long);
typedef long ret_fn_t (void);

static int n;
static long last_arg;
int other;

static __attribute__ ((noinline)) void
impl (void)
{
  __asm__ ("" : : : "memory");
  n++;
}

/* The alias is declared ahead of the ifunc symbol, which means the ifunc symbol
   gets resolved second and mustn't drag the alias along to the resolver.  */
extern fn_t rev_alias __attribute__ ((alias ("rev_ifunc")));
static fn_t *rev_res (void) { return impl; }
extern fn_t rev_ifunc __attribute__ ((ifunc ("rev_res")));

/* The basic case, and the one whose addresses get compared.  */
static fn_t *plain_res (void) { return impl; }
extern fn_t plain_ifunc __attribute__ ((ifunc ("plain_res")));
extern fn_t plain_alias __attribute__ ((alias ("plain_ifunc")));

/* Alias chains of every kind.  */
static fn_t *chain_res (void) { return impl; }
extern fn_t chain_ifunc __attribute__ ((ifunc ("chain_res")));
extern fn_t chain_a1 __attribute__ ((alias ("chain_ifunc")));
extern fn_t chain_a2 __attribute__ ((alias ("chain_a1")));
extern fn_t chain_a3 __attribute__ ((alias ("chain_a2")));
extern fn_t chain_weak __attribute__ ((weak, alias ("chain_ifunc")));
static fn_t chain_static __attribute__ ((alias ("chain_ifunc")));
static fn_t chain_weakref __attribute__ ((weakref ("chain_ifunc")));
static fn_t chain_weakref2 __attribute__ ((weakref ("chain_a2")));

/* The ifunc symbol names its resolver through an alias of the resolver.  That
   alias is a plain function name, and calling it has to call the resolver.  */
static fn_t *res_alias_res (void) { return impl; }
extern fn_t *res_alias (void) __attribute__ ((alias ("res_alias_res")));
extern fn_t res_alias_ifunc __attribute__ ((ifunc ("res_alias")));
extern fn_t res_alias_alias __attribute__ ((alias ("res_alias_ifunc")));

/* Two ifunc symbols sharing one resolver.  */
static fn_t *two_res (void) { return impl; }
extern fn_t two_a __attribute__ ((ifunc ("two_res")));
extern fn_t two_b __attribute__ ((ifunc ("two_res")));
extern fn_t two_a_alias __attribute__ ((alias ("two_a")));
extern fn_t two_b_alias __attribute__ ((alias ("two_b")));

/* The always_inline inliner runs even at -O0.  */
static __inline__ __attribute__ ((always_inline)) fn_t *
ai_res (void)
{
  return impl;
}
extern fn_t ai_ifunc __attribute__ ((ifunc ("ai_res")));
extern fn_t ai_alias __attribute__ ((alias ("ai_ifunc")));

/* Everything local to this file.  */
static __inline__ __attribute__ ((always_inline)) fn_t *
st_res (void)
{
  return impl;
}
static fn_t st_ifunc __attribute__ ((ifunc ("st_res")));
static fn_t st_alias __attribute__ ((alias ("st_ifunc")));

/* The return value belongs to the picked function, whereas the resolver always
   returns nonnull.  */
static __attribute__ ((noinline)) long
ret_impl (void)
{
  __asm__ ("" : : : "memory");
  return 0;
}
static __attribute__ ((noinline)) ret_fn_t *ret_res (void) { return ret_impl; }
extern ret_fn_t ret_ifunc __attribute__ ((ifunc ("ret_res")));
extern ret_fn_t ret_alias __attribute__ ((alias ("ret_ifunc")));

/* The argument goes to the picked function and mustn't be propagated into the
   resolver, which would make it return null in a clone.  */
#define ARG_VALUE 0x12345678L
static __attribute__ ((noinline)) void
arg_impl (long x)
{
  __asm__ ("" : : : "memory");
  last_arg = x;
}
static __attribute__ ((noinline)) arg_fn_t *
arg_res (long x)
{
  return x == ARG_VALUE ? (arg_fn_t *) 0 : arg_impl;
}
extern arg_fn_t arg_ifunc __attribute__ ((ifunc ("arg_res")));
extern arg_fn_t arg_alias __attribute__ ((alias ("arg_ifunc")));

/* Pure-const discovery would see a call to the resolver here, which is const,
   and then delete the calls to these functions.  */
__attribute__ ((noinline)) int
const_direct (void)
{
  plain_ifunc ();
  return 1;
}

__attribute__ ((noinline)) int
const_alias (void)
{
  plain_alias ();
  return 1;
}

/* These can't be const, but ipa-reference would decide they don't touch N.  */
__attribute__ ((noinline)) void
reference_direct (void)
{
  other++;
  plain_ifunc ();
}

__attribute__ ((noinline)) void
reference_alias (void)
{
  other++;
  plain_alias ();
}

/* Calls through pointers, where IPA points-to analysis has to connect the
   pointer to the picked function rather than to the ifunc symbol.  It would
   otherwise miss the picked function's side effects and parameters.  */
static __attribute__ ((noinline)) void
call_ptr (fn_t *f)
{
  f ();
}

static fn_t *const table[] = { plain_ifunc, plain_alias };

static __attribute__ ((noinline)) void
call_table (int i)
{
  table[i] ();
}

typedef void ptr_fn_t (int *);

static __attribute__ ((noinline)) void
ptr_impl (int *p)
{
  __asm__ ("" : : : "memory");
  *p = 1;
}
static ptr_fn_t *ptr_res (void) { return ptr_impl; }
extern ptr_fn_t ptr_ifunc __attribute__ ((ifunc ("ptr_res")));
extern ptr_fn_t ptr_alias __attribute__ ((alias ("ptr_ifunc")));

static __attribute__ ((noinline)) void
call_ptr_arg (ptr_fn_t *f, int *p)
{
  f (p);
}

__attribute__ ((noinline, flatten)) void
flatten_calls (void)
{
  plain_ifunc ();
  plain_alias ();
}

/* Run CALLS and make sure the picked function ran COUNT times.  */
#define CHECK(CALLS, COUNT)			\
  do						\
    {						\
      n = 0;					\
      CALLS;					\
      if (n != (COUNT))				\
	__builtin_abort ();			\
    }						\
  while (0)

/* Whether two names compare equal can depend on how the target materializes the
   address of an ifunc symbol.  A comparison folded at compile time still has to
   match what the addresses do at run time.  */
#define CHECK_COMPARE(A, B)				\
  do							\
    {							\
      fn_t *volatile a_ = (fn_t *) (A);			\
      fn_t *volatile b_ = (fn_t *) (B);			\
      if (((fn_t *) (A) == (fn_t *) (B)) != (a_ == b_))	\
	__builtin_abort ();				\
    }							\
  while (0)

int
main (void)
{
  int ptr_arg_set;

  CHECK (plain_ifunc (), 1);
  CHECK (plain_alias (), 1);
  CHECK ((plain_ifunc (), plain_alias ()), 2);

  CHECK (rev_ifunc (), 1);
  CHECK (rev_alias (), 1);

  CHECK (chain_ifunc (), 1);
  CHECK (chain_a1 (), 1);
  CHECK (chain_a2 (), 1);
  CHECK (chain_a3 (), 1);
  CHECK (chain_weak (), 1);
  CHECK (chain_static (), 1);
  CHECK (chain_weakref (), 1);
  CHECK (chain_weakref2 (), 1);

  CHECK (res_alias_ifunc (), 1);
  CHECK (res_alias_alias (), 1);
  CHECK (if (res_alias () != impl) __builtin_abort (), 0);
  CHECK (if (res_alias_res () != impl) __builtin_abort (), 0);

  CHECK ((two_a (), two_b (), two_a_alias (), two_b_alias ()), 4);

  CHECK ((ai_ifunc (), ai_alias ()), 2);
  CHECK ((st_ifunc (), st_alias ()), 2);

  if (ret_ifunc () != 0 || ret_alias () != 0)
    __builtin_abort ();

  arg_alias (ARG_VALUE);
  if (last_arg != ARG_VALUE)
    __builtin_abort ();
  last_arg = 0;
  arg_ifunc (ARG_VALUE);
  if (last_arg != ARG_VALUE)
    __builtin_abort ();

  CHECK ((const_direct (), const_direct ()), 2);
  CHECK ((const_alias (), const_alias ()), 2);
  CHECK (reference_direct (), 1);
  CHECK (reference_alias (), 1);
  CHECK ((call_ptr (plain_ifunc), call_ptr (plain_alias)), 2);
  CHECK ((call_table (0), call_table (1)), 2);

  ptr_arg_set = 0;
  call_ptr_arg (ptr_ifunc, &ptr_arg_set);
  if (ptr_arg_set != 1)
    __builtin_abort ();
  ptr_arg_set = 0;
  call_ptr_arg (ptr_alias, &ptr_arg_set);
  if (ptr_arg_set != 1)
    __builtin_abort ();

  CHECK (flatten_calls (), 2);

  /* Neither name of an ifunc symbol is ever the address of its resolver.  */
  if ((fn_t *) plain_ifunc == (fn_t *) plain_res
      || (fn_t *) plain_alias == (fn_t *) plain_res
      || (fn_t *) chain_a3 == (fn_t *) chain_res)
    __builtin_abort ();

  CHECK_COMPARE (plain_ifunc, impl);
  CHECK_COMPARE (plain_alias, impl);
  CHECK_COMPARE (plain_alias, plain_ifunc);
  CHECK_COMPARE (two_a, two_b);
  CHECK_COMPARE (two_a_alias, two_b_alias);

  return 0;
}
