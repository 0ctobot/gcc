/* PR ipa/95558 */
/* { dg-do run } */
/* { dg-require-weak-override "" } */
/* { dg-require-alias "" } */
/* { dg-additional-sources "pr95558-3a.c" } */
/* { dg-additional-options "-fipa-pta -std=gnu11" } */

/* A symbol that the linker can resolve to another definition says nothing about
   what's behind it, regardless of the definition in this unit.  The definitions
   in pr95558-3a.c override the weak symbols here.  IPA-PTA must treat a
   reference to each of them, direct or through a pointer, like a reference to
   an external symbol.  */

extern void abort (void);

int g;
int a, b;
static int s;
static int v;

void
set_v (void)
{
  v = 1;
}

/* What a call can write to.  */
static void
empty (void)
{
}
extern void weak_empty (void) __attribute__ ((weak, alias ("empty")));

__attribute__ ((weak)) void
weak_def_empty (void)
{
}

/* What a call can return.  */
static int *
ret_s (void)
{
  return &s;
}
extern int *weak_ret_s (void) __attribute__ ((weak, alias ("ret_s")));

__attribute__ ((weak)) int *
weak_def_ret_s (void)
{
  return &s;
}

/* The noipa attribute doesn't stop a symbol from being replaced.  */
__attribute__ ((weak, noipa)) int *
weak_noipa_def_ret_s (void)
{
  return &s;
}

__attribute__ ((noipa)) static int *
noipa_ret_s (void)
{
  return &s;
}
extern int *weak_noipa_ret_s (void)
  __attribute__ ((weak, alias ("noipa_ret_s")));

/* What a call can do with its argument.  */
static void
ignore_arg (int *p)
{
  (void) p;
}
extern void weak_ignore_arg (int *p)
  __attribute__ ((weak, alias ("ignore_arg")));

__attribute__ ((weak)) void
weak_def_ignore_arg (int *p)
{
  (void) p;
}

/* The definition here has no parameters to pass the argument to.  */
__attribute__ ((weak)) void
weak_def_no_params ()
{
}

/* Other units can get at these through their aliases.  */
static int *sp = &a;
extern int *weak_sp __attribute__ ((weak, alias ("sp")));
static int *sp2 = &a;
extern int *sp2_alias __attribute__ ((alias ("sp2")));

/* The set_ptrs function keeps these from becoming constant.  */
static void (*empty_alias_ptr) (void) = weak_empty;
static void (*empty_def_ptr) (void) = weak_def_empty;
static int *(*ret_alias_ptr) (void) = weak_ret_s;
static int *(*ret_def_ptr) (void) = weak_def_ret_s;
static void (*arg_alias_ptr) (int *) = weak_ignore_arg;
static void (*arg_def_ptr) (int *) = weak_def_ignore_arg;

/* A call through a pointer to an external function gets the same treatment.  */
extern int *ext_ret (void);
static int *(*ret_ext_ptr) (void) = ext_ret;

void
set_ptrs (void)
{
  empty_alias_ptr = weak_empty;
  empty_def_ptr = weak_def_empty;
  ret_alias_ptr = weak_ret_s;
  ret_def_ptr = weak_def_ret_s;
  arg_alias_ptr = weak_ignore_arg;
  arg_def_ptr = weak_def_ignore_arg;
  ret_ext_ptr = ext_ret;
}

#define TEST_CLOBBER(NAME, CALLEE)		\
  __attribute__ ((noinline)) int		\
  NAME (void)					\
  {						\
    void (*fp) (void) = CALLEE;			\
    v = 0;					\
    fp ();					\
    return v;					\
  }

#define TEST_RET(NAME, CALLEE)			\
  __attribute__ ((noinline)) int		\
  NAME (void)					\
  {						\
    int *(*fp) (void) = CALLEE;			\
    int *p = fp ();				\
    g = 1;					\
    *p = 2;					\
    return g;					\
  }

#define TEST_ARG(NAME, CALLEE)			\
  __attribute__ ((noinline)) int		\
  NAME (void)					\
  {						\
    void (*fp) (int *) = CALLEE;		\
    int x = 1;					\
    fp (&x);					\
    return x;					\
  }

TEST_CLOBBER (test_clobber_alias, weak_empty)
TEST_CLOBBER (test_clobber_def, weak_def_empty)
TEST_CLOBBER (test_clobber_alias_ptr, empty_alias_ptr)
TEST_CLOBBER (test_clobber_def_ptr, empty_def_ptr)
TEST_RET (test_ret_alias, weak_ret_s)
TEST_RET (test_ret_def, weak_def_ret_s)
TEST_RET (test_ret_alias_ptr, ret_alias_ptr)
TEST_RET (test_ret_def_ptr, ret_def_ptr)
TEST_RET (test_ret_ext_ptr, ret_ext_ptr)
TEST_RET (test_ret_noipa_def, weak_noipa_def_ret_s)
TEST_RET (test_ret_noipa_alias, weak_noipa_ret_s)
TEST_ARG (test_arg_alias, weak_ignore_arg)
TEST_ARG (test_arg_def, weak_def_ignore_arg)
TEST_ARG (test_arg_alias_ptr, arg_alias_ptr)
TEST_ARG (test_arg_def_ptr, arg_def_ptr)

/* The call directly names its callee to keep the excess argument.  */
__attribute__ ((noinline)) int
test_arg_no_params (void)
{
  int x = 1;
  weak_def_no_params (&x);
  return x;
}

__attribute__ ((noinline)) int
test_var_weak (void)
{
  b = 1;
  *weak_sp = 2;
  return b;
}

__attribute__ ((noinline)) int
test_var_alias (void)
{
  b = 1;
  *sp2_alias = 2;
  return b;
}

extern void set_sp2_alias (void);

int
main (void)
{
  if (test_clobber_alias () != 1
      || test_clobber_def () != 1
      || test_clobber_alias_ptr () != 1
      || test_clobber_def_ptr () != 1)
    abort ();
  if (test_ret_alias () != 2
      || test_ret_def () != 2
      || test_ret_alias_ptr () != 2
      || test_ret_def_ptr () != 2
      || test_ret_ext_ptr () != 2
      || test_ret_noipa_def () != 2
      || test_ret_noipa_alias () != 2)
    abort ();
  if (test_arg_alias () != 2
      || test_arg_def () != 2
      || test_arg_alias_ptr () != 2
      || test_arg_def_ptr () != 2
      || test_arg_no_params () != 2)
    abort ();
  set_sp2_alias ();
  if (test_var_weak () != 2
      || test_var_alias () != 2)
    abort ();
  return 0;
}
