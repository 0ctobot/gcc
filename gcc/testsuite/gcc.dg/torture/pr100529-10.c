/* PR c/100529 */
/* { dg-do run } */
/* { dg-options "-std=gnu23" } */

/* The sizes in the type of the result of a conditional whose arms have
   variably sized struct types of the same tag come from the arm that the
   condition selects, and code after the conditional can use any of them,
   including the sizes of a pointer member's target type.  */

extern void abort (void);

__attribute__ ((noipa)) int
gn (void)
{
  return 4;
}

/* Both arms declare their own type sized from the same outer variable.  */

__attribute__ ((noipa)) int
both_outer (int c, int n)
{
  return (c ? ({ struct T { char a[n]; } t; t.a[0] = 1; t; })
	  : ({ struct T { char a[n]; } t; t.a[0] = 2; t; })).a[0];
}

__attribute__ ((noipa)) long
both_outer_size (int c, int n)
{
  return sizeof (c ? ({ struct T { char a[n]; } t; t; })
		 : ({ struct T { char a[n]; } t; t; }));
}

/* One arm's type is sized from a variable local to it, the other's from an
   outer variable.  */

__attribute__ ((noipa)) int
local_and_outer (int c, int n)
{
  return (c ? ({ int m = gn (); struct T { char a[m]; int b; } t; t.b = 11; t; })
	  : ({ struct T { char a[n]; int b; } t; t.b = 22; t; })).b;
}

/* A conditional nested in an arm, whose other arm has the outer type.  */

__attribute__ ((noipa)) int
nested (int c, int d, int n)
{
  struct T { char a[n]; int b; } o;
  o.b = 77;
  return (c ? ({ int m = gn (); struct T { char a[m]; int b; } t;
		 t.b = 11; (d ? t : o); }) : o).b;
}

/* A constant size struct with a pointer to an array sized in an arm.  */

__attribute__ ((noipa)) long
pointer_to_vla (int c, int n)
{
  char b[n];
  struct T { char (*p)[n]; int k; } o = { &b, 1 };
  return sizeof *(c ? ({ int m = gn (); struct T { char (*p)[m]; int k; } t
			 = { (void *) &b, 2 }; t; }) : o).p;
}

/* A constant size struct with a pointer to a struct sized in an arm.  */

__attribute__ ((noipa)) int
pointer_to_struct (int c, int n)
{
  int m0 = n;
  struct T { char a[m0]; int b; } o;
  o.b = 77;
  struct W { struct T *q; int e; } wo = { &o, 3 };
  return (c ? ({ int m = gn (); struct T { char a[m]; int b; };
		 struct W { struct T *q; int e; } t = { (void *) &o, 4 }; t; })
	  : wo).q->b;
}

/* The value of the conditional is discarded or only its type is used.  */

__attribute__ ((noipa)) long
discarded (int c, int n)
{
  struct T { char a[n]; int b; } o;
  o.b = 77;
  (void) (c ? ({ int m = gn (); struct T { char a[m]; int b; } t;
		 t.b = 11; t; }) : o);
  typeof (c ? ({ int m = gn (); struct T { char a[m]; int b; } t;
		 t.b = 11; t; }) : o) r;
  return sizeof r;
}

int
main (void)
{
  if (both_outer (1, 4) != 1 || both_outer (0, 4) != 2)
    abort ();
  if (both_outer_size (1, 4) != 4 || both_outer_size (0, 5) != 5)
    abort ();
  if (local_and_outer (1, 4) != 11 || local_and_outer (0, 4) != 22)
    abort ();
  if (nested (1, 1, 4) != 11 || nested (1, 0, 4) != 77
      || nested (0, 0, 4) != 77)
    abort ();
  if (pointer_to_vla (0, 4) != 4 || pointer_to_vla (1, 4) != 4)
    abort ();
  if (pointer_to_struct (0, 4) != 77 || pointer_to_struct (1, 4) != 77)
    abort ();
  if (discarded (0, 4) != 8 || discarded (1, 4) != 8)
    abort ();
  return 0;
}
