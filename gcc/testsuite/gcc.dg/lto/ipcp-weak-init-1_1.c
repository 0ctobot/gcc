struct ops { int id; int (*fn) (int); };

static int cb_b (int v) { return v + 1; }

/* Replaces the weak definition in the other file.  */
const struct ops T = { 2, cb_b };

extern int use (int);

int
main (void)
{
  /* cb_b gives 1 + 2 + 3; the weak table's cb_a would give 3003.  */
  if (use (3) != 6)
    __builtin_abort ();
  return 0;
}
