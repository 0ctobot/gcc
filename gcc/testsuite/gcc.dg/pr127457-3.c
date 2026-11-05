/* PR middle-end/127457 */
/* { dg-do run } */
/* { dg-options "-O2" } */

/* Inlining puts the helper's string tree into both functions.  The -Os
   function places the string with less alignment than the -O2 function needs,
   so the -O2 function gets a copy of its own, and that copy has to be emitted
   too.  The string is long enough for x86 to raise its alignment as well.  */

static inline __attribute__((always_inline)) const char *
text (void)
{
  return "a string long enough to be word-aligned";
}

__attribute__((noipa, no_reorder, optimize ("Os"))) const char *
os_text (void)
{
  return text ();
}

__attribute__((noipa, no_reorder)) const char *
o2_text (void)
{
  return text ();
}

int
main (void)
{
  if (__builtin_strcmp (os_text (), o2_text ()) != 0)
    __builtin_abort ();
  return 0;
}
