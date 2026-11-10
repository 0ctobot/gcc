/* { dg-do compile } */
/* { dg-options "-ftrivial-auto-var-init=zero -fdump-tree-gimple" } */

/* Only an initializer that can read the variable it initializes needs the
   automatic initialization ahead of it.  Taking the address alone doesn't
   read anything.  */

struct list_head { struct list_head *next, *prev; };
extern int use (void *);
extern int read_int (int *);

int
self_read (int c)
{
  int x = x;
  if (c)
    x = 1;
  return x;
}

int
self_call (void)
{
  int x = read_int (&x);
  return x;
}

int
self_addr (void)
{
  struct list_head l = { &l, &l };
  return use (&l);
}

int
plain (int y)
{
  int x = y + 1;
  return use (&x);
}

/* { dg-final { scan-tree-dump-times "DEFERRED_INIT" 2 "gimple" } } */
