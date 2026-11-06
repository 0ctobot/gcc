/* PR ipa/95558 */
/* { dg-do run } */
/* { dg-require-weak-override "" } */
/* { dg-require-alias "" } */
/* { dg-additional-sources "pr95558-1a.c" } */

/* A call through a weak alias runs whichever definition the linker picks, which
   means the function that the alias names in this unit says nothing about the
   call.  The definitions in pr95558-1a.c override the weak symbols here.  */

extern void abort (void);

int calls;
int g;

/* The empty body must not make its callers const or pure.  */
static void
empty (void)
{
}
extern void weak_empty (void) __attribute__ ((weak, alias ("empty")));

static __attribute__ ((noinline)) void
const_caller (void)
{
  weak_empty ();
}

/* Reading g would make this pure rather than const.  */
static __attribute__ ((noinline)) int
pure_caller (void)
{
  weak_empty ();
  return g;
}

/* The malloc-like body must not make its callers malloc-like.  */
static int *
malloc_like (void)
{
  return __builtin_malloc (sizeof (int));
}
extern int *weak_malloc_like (void)
  __attribute__ ((weak, alias ("malloc_like")));

__attribute__ ((noinline)) int *
weak_malloc_caller (int c)
{
  return c ? weak_malloc_like () : __builtin_malloc (sizeof (int));
}

__attribute__ ((noinline)) int
test_weak_malloc (void)
{
  int *p;

  g = 1;
  p = weak_malloc_caller (1);
  if (!p)
    return 0;
  *p = 2;
  return g;
}

/* Nothing overrides this alias, but the body it names isn't malloc-like.  */
static __attribute__ ((noinline)) int *
not_malloc_like (void)
{
  return &g;
}
extern int *alias_not_malloc_like (void)
  __attribute__ ((alias ("not_malloc_like")));

__attribute__ ((noinline)) int *
alias_caller (int c)
{
  return c ? alias_not_malloc_like () : __builtin_malloc (sizeof (int));
}

__attribute__ ((noinline)) int
test_alias (void)
{
  int *p;

  g = 1;
  p = alias_caller (1);
  if (!p)
    return 0;
  *p = 2;
  return g;
}

int
main (void)
{
  const_caller ();
  if (calls != 1)
    abort ();
  pure_caller ();
  if (calls != 2)
    abort ();
  if (test_weak_malloc () != 2)
    abort ();
  if (test_alias () != 2)
    abort ();
  return 0;
}
