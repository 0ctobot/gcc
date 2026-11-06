extern int test (int *);

int alias_weakref_target;

int
main (void)
{
  if (test (&alias_weakref_target) != 2)
    __builtin_abort ();
  return 0;
}
