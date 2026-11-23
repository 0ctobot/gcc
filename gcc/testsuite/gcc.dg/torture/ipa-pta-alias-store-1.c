/* { dg-do run } */
/* { dg-require-alias "" } */
/* { dg-additional-options "-fipa-pta" } */
/* { dg-additional-sources "ipa-pta-alias-store-1a.c" } */

/* Storing a pointer through an alias of a variable that code outside of this
   unit can read makes the pointer escape, just like a store through the
   variable's own name.  The functions in ipa-pta-alias-store-1a.c write through
   the stored pointers.  */

int *sp;
extern int *sp_alias __attribute__ ((alias ("sp")));

static int *hidden;
extern int *hidden_alias __attribute__ ((alias ("hidden")));

extern void ext (void);
extern void ext_hidden (void);

__attribute__ ((noinline)) int
test_public (void)
{
  int x = 1;
  sp_alias = &x;
  ext ();
  return x;
}

__attribute__ ((noinline)) int
test_static (void)
{
  int x = 1;
  hidden_alias = &x;
  ext_hidden ();
  return x;
}

int
main (void)
{
  if (test_public () != 2)
    __builtin_abort ();
  if (test_static () != 2)
    __builtin_abort ();
  return 0;
}
