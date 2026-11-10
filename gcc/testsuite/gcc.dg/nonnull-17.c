/* -fno-delete-null-pointer-checks keeps a null check on a parameter that
   the nonnull attribute (or [static N]) says can't be null.  PR101665.  */
/* { dg-do run } */
/* { dg-options "-O2 -fno-delete-null-pointer-checks" } */

struct node { struct node *next; int v; };

__attribute__((noipa)) void *
hide (void *p)
{
  return p;
}

__attribute__((nonnull, noipa)) int
nn1 (int *p)
{
  if (!p)
    return -1;
  return 1;
}

__attribute__((nonnull, noipa)) int
count (struct node *p)
{
  int c = 0;
  for (; p; p = p->next)
    c++;
  return c;
}

__attribute__((noipa)) int
static_param (int p[static 1])
{
  if (!p)
    return -1;
  return p[0];
}

int
main (void)
{
  if (nn1 (hide (0)) != -1)
    __builtin_abort ();
  if (count (hide (0)) != 0)
    __builtin_abort ();
  if (static_param (hide (0)) != -1)
    __builtin_abort ();
  return 0;
}
