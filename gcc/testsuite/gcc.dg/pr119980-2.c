/* PR target/119980 */
/* { dg-do run } */
/* { dg-options "-O2" } */

/* Like pr119980.c with char members.  A struct having a member with alias set
   zero doesn't make other structs subsets of it, so the store through F has to
   stay.  */

typedef struct foo { char x; } F;
typedef struct bar { char x; } B;

static int
foo (void *p, int i, int j, char v)
{
  F *pp = p;
  pp[i] = (F) { v };
  return pp[j].x;
}

static int
bar (void *p, int i, int j, char v)
{
  B *pp = p;
  pp[i] = (B) { v };
  return pp[j].x;
}

__attribute__((noinline)) int
test (void *fp, int i, int j, int k)
{
  foo (fp, i, j, 1);
  int s = bar (fp, j, k, 2);
  return foo (fp, k, i, s);
}

/* The store through the char pointer conflicts with the load through IP, while
   the store through the struct member doesn't.  */

__attribute__((noinline)) int
test2 (B *p, int *ip, char v)
{
  *ip = 0x07070707;
  p->x = v;
  *(char *) p = v;
  return *ip;
}

int
main (void)
{
  void *fp = __builtin_malloc (sizeof (int));
  if (fp && test (fp, 0, 0, 0) != 2)
    __builtin_abort ();
  if (fp && test2 (fp, fp, 1) == 0x07070707)
    __builtin_abort ();
  return 0;
}
