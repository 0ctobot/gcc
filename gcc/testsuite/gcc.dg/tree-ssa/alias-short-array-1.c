/* A constant index inside the bounds of an array still lets the alias
   oracle separate two references by the fields that follow the array
   reference.  With -fstrict-aliasing, nonoverlapping_component_refs_p ()
   would separate them too, so it's disabled here.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fno-strict-aliasing -fdump-tree-fre1" } */

struct e { int f; int g; };
struct s { struct e arr[4]; int tail; };

int
in_bounds (struct s *p, long i)
{
  p->arr[i].g = 1;
  p->arr[1].f = 2;
  return p->arr[i].g;
}

/* { dg-final { scan-tree-dump "return 1;" "fre1" } } */
