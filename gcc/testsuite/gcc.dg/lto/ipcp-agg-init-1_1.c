extern int use_flat (int);

__attribute__ ((noipa)) int
sink (int v)
{
  return v;
}

/* Keep the count unknown, so that only the tables are worth specializing
   run_flat for.  */
volatile int count = 4;

int
main (void)
{
  /* cb_a over 0..3 gives 22 and cb_b over 0..4 gives 60.  */
  if (use_flat (count) != 82)
    __builtin_abort ();
  return 0;
}
