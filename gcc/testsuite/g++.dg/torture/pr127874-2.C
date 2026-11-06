// PR tree-optimization/127874
// { dg-do run }

// A namespace-scope initializer calls strlen, which is declared noreturn and
// bound to my_strlen.  Constant evaluation mustn't fold the call, which has
// to run during dynamic initialization and exit before main is reached.

#define STR1(x) #x
#define STR(x) STR1 (x)

extern "C" __SIZE_TYPE__ strlen (const char *)
  __asm__ (STR (__USER_LABEL_PREFIX__) "my_strlen") __attribute__ ((noreturn));

extern "C" __attribute__ ((used)) __SIZE_TYPE__
my_strlen (const char *)
{
  __builtin_exit (0);
}

__SIZE_TYPE__ n = strlen ("abc");

int
main ()
{
  __builtin_abort ();
}
