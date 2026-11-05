/* PR middle-end/127457 */
/* { dg-do compile } */
/* { dg-options "-O2 -fcompare-debug" } */

/* The -Os function places the string with less alignment than an -O2 function
   needs.  A debug bind in an -O2 function must not place a better aligned copy
   of its own, since that would change the labels of the constants that follow
   only when compiling with -g.  The string is long enough for x86 to raise its
   alignment as well.  */

extern void use (const char *);

#define STR "a string long enough to be word-aligned on x86 too"

__attribute__ ((noipa, no_reorder, optimize ("Os"))) void
os_first (void)
{
  use (STR);
}

__attribute__ ((noipa, no_reorder)) void
o2_debug_only (void)
{
  const char *p = STR;
  (void) p;
}

__attribute__ ((noipa, no_reorder)) void
o2_later (void)
{
  use ("another string that takes the next free constant label");
}
