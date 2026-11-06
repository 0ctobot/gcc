// PR tree-optimization/127874
// { dg-do run }

// The initializer of a local const integer goes through constant evaluation
// too, and the noreturn call of strlen mustn't be folded there either.

#define STR1(x) #x
#define STR(x) STR1 (x)

extern "C" __SIZE_TYPE__ strlen (const char *)
  __asm__ (STR (__USER_LABEL_PREFIX__) "my_strlen") __attribute__ ((noreturn));

extern "C" __attribute__ ((used)) __SIZE_TYPE__
my_strlen (const char *)
{
  __builtin_exit (0);
}

__attribute__ ((noipa)) __SIZE_TYPE__
f ()
{
  const __SIZE_TYPE__ n = strlen ("abc");
  return n;
}

int
main ()
{
  f ();
  __builtin_abort ();
}
