/* The selective scheduler used to move a load into a register above a
   conditional jump while that register was live on the jump's other path,
   since a jump-only block created for bookkeeping kept a stale live set.  */
/* { dg-do run } */
/* { dg-require-effective-target scheduling } */
/* { dg-options "-O2 -fno-tree-fre -fno-inline -fselective-scheduling2" } */

int printf (const char *, ...);
int a, b, c = -3, h, j, k;
struct d
{
  int e;
  char f;
  char g;
};
int i, l, m;
char n[10][9] = {{1}};
struct d o = {1, 0, 0};
unsigned p;

void
q (int s)
{
  b = a;
}

void
r (void)
{
  k = 0;
  while (k)
    while (l)
      while (i)
	if (m)
	  while (1)
	    ;
  for (; p < 1; p++)
    {
      if (j)
	{
	  q (n[j][c]);
	  if (h)
	    printf ("\n");
	}
      n[p][k] = 1;
    }
}

int
main (void)
{
  r ();
  if (o.g != 0)
    __builtin_abort ();
  return 0;
}
