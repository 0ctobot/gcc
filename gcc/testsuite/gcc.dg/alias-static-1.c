/* The address of a static variable can be taken through an alias of it, so
   a pointer obtained that way may point to the static variable even though it
   never has its own address taken.  */
/* { dg-do run } */
/* { dg-options "-O2" } */
/* { dg-require-alias "" } */
/* { dg-require-weak "" } */

#define TEST(n, pre_decls, alias_name)					\
  pre_decls								\
  __attribute__ ((noinline)) int *					\
  get##n (void)								\
  {									\
    __asm__ ("" : : : "memory");					\
    return &alias_name;							\
  }									\
  __attribute__ ((noinline)) int					\
  test##n (void)							\
  {									\
    int *p = get##n ();							\
    s##n = 1;								\
    *p = 2;								\
    return s##n;							\
  }

/* A public alias.  */
TEST (1, static int s1; extern int a1 __attribute__ ((alias ("s1")));, a1)

/* The alias comes before the variable.  */
TEST (2, extern int a2 __attribute__ ((alias ("s2"))); static int s2;, a2)

/* A chain of aliases.  */
TEST (3, static int s3; extern int b3 __attribute__ ((alias ("s3")));
	 extern int a3 __attribute__ ((alias ("b3")));, a3)

/* A weak alias.  */
TEST (4, static int s4; extern int a4 __attribute__ ((weak, alias ("s4")));, a4)

/* A static alias.  */
TEST (5, static int s5; static int a5 __attribute__ ((alias ("s5")));, a5)

/* The store is out of sight in a function that ipa-reference looks at.  */
static int s6;
extern int a6 __attribute__ ((alias ("s6")));

__attribute__ ((noinline)) int *
get6 (void)
{
  __asm__ ("" : : : "memory");
  return &a6;
}

__attribute__ ((noinline)) void
store6 (int *p)
{
  *p = 2;
}

__attribute__ ((noinline)) int
test6 (void)
{
  int *p = get6 ();
  s6 = 1;
  store6 (p);
  return s6;
}

/* The address of a static alias is taken in an initializer.  */
static int s7;
static int a7 __attribute__ ((alias ("s7")));
static int *const volatile p7 = &a7;

__attribute__ ((noinline)) int
test7 (void)
{
  int *p = p7;
  s7 = 1;
  *p = 2;
  return s7;
}

int
main (void)
{
  if (test1 () != 2 || test2 () != 2 || test3 () != 2 || test4 () != 2
      || test5 () != 2 || test6 () != 2 || test7 () != 2)
    __builtin_abort ();
  return 0;
}
