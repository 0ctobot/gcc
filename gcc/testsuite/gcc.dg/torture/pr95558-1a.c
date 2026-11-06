/* { dg-do compile } */

extern int calls;
extern int g;

void
weak_empty (void)
{
  calls++;
}

/* Keep this out of line since LTO can see this override, and the malloc check
   is about what happens to a call that survives.  */
__attribute__ ((noinline)) int *
weak_malloc_like (void)
{
  return &g;
}
