/* { dg-do run } */
/* { dg-options "-O2" } */

/* A variable index that may be negative only takes the static maximum for
   a member to the whole object when a negative index can leave the pointer
   inside the object.  When it can only take the pointer out of a declared
   object, the static maximum stays the member's.  */

typedef __SIZE_TYPE__ size_t;

#ifdef PRINT
int printf (const char *, ...);
#define EQ(expr, want) \
  printf ("%-28s %4zu  want == %zu\n", #expr, (size_t) (expr), (size_t) (want))
#define GE(expr, want) \
  printf ("%-28s %4zu  want >= %zu\n", #expr, (size_t) (expr), (size_t) (want))
#else
#define EQ(expr, want) if ((expr) != (want)) __builtin_abort ()
#define GE(expr, want) if ((expr) < (want)) __builtin_abort ()
#endif

struct ent { int id; char name[16]; };
struct tbl { struct ent a[4]; int after[16]; } g;
struct tbl2 { int hdr; struct ent a[4]; } g2;
struct tbl3 { char hdr[24]; struct ent a[4]; } g3;
struct mat { char m[4][8]; int t; } gm;
struct mat2 { char pre[8]; char m[4][8]; } gm2;
struct row { struct ent arr[2]; };
struct rows { struct row ar[2]; } gr;
struct outer { struct tbl2 b[2]; } go;
struct ent2 { char tag[4]; char name[16]; };
struct tbl15 { char hdr[15]; struct ent2 a[4]; } g15;
struct tbl16 { char hdr[16]; struct ent2 a[4]; } g16;

/* A negative index takes the pointer out of the object.  */

__attribute__ ((noipa)) size_t
n_int (int i)
{
  return __builtin_object_size (g.a[i].name, 1);
}

__attribute__ ((noipa)) size_t
n_long (long i)
{
  return __builtin_object_size (g.a[i].name, 1);
}

__attribute__ ((noipa)) size_t
n_short (short i)
{
  return __builtin_object_size (&g.a[i].name[4], 1);
}

__attribute__ ((noipa)) size_t
n_hdr (int i)
{
  return __builtin_object_size (g2.a[i].name, 1);
}

__attribute__ ((noipa)) size_t
n_row (int i)
{
  return __builtin_object_size (&gm.m[i][3], 1);
}

/* Index -1 takes the address one byte below g15, and three bytes below
   g16 for a[i].tag[1].  */

__attribute__ ((noipa)) size_t
n_edge (int i)
{
  return __builtin_object_size (g15.a[i].name, 1);
}

__attribute__ ((noipa)) size_t
n_tag (int i)
{
  return __builtin_object_size (&g16.a[i].tag[1], 1);
}

/* These kept the member's size before.  */

__attribute__ ((noipa)) size_t
k_unsigned (unsigned i)
{
  return __builtin_object_size (g.a[i].name, 1);
}

__attribute__ ((noipa)) size_t
k_ptr (struct tbl *p, int i)
{
  return __builtin_object_size (p->a[i].name, 1);
}

__attribute__ ((noipa)) size_t
k_plus (int i)
{
  return __builtin_object_size (g.a[2].name + i, 1);
}

/* A negative index can leave the pointer inside the object, so the static
   maximum has to cover what's left from the lowest such address.  */

__attribute__ ((noipa)) size_t
w_hdr (int i)
{
  return __builtin_object_size (g3.a[i].name, 1);
}

__attribute__ ((noipa)) size_t
w_row (int i)
{
  return __builtin_object_size (&gm2.m[i][3], 1);
}

__attribute__ ((noipa)) size_t
w_inner (int i)
{
  return __builtin_object_size (gr.ar[1].arr[i].name, 1);
}

__attribute__ ((noipa)) size_t
w_outer (unsigned j, int i)
{
  return __builtin_object_size (go.b[j].a[i].name, 1);
}

/* Index -1 takes the address to the start of g16.  */

__attribute__ ((noipa)) size_t
w_edge (int i)
{
  return __builtin_object_size (g16.a[i].name, 1);
}

__attribute__ ((noipa)) size_t
w_const (void)
{
  return __builtin_object_size (g3.a[-1].name, 1);
}

/* The dynamic size goes by the index at run time.  */

__attribute__ ((noipa)) size_t
d_int (int i)
{
  return __builtin_dynamic_object_size (g.a[i].name, 1);
}

__attribute__ ((noipa)) size_t
d_row (int i)
{
  return __builtin_dynamic_object_size (&gm.m[i][3], 1);
}

__attribute__ ((noipa)) size_t
d_hdr (int i)
{
  return __builtin_dynamic_object_size (g3.a[i].name, 1);
}

__attribute__ ((noipa)) size_t
d_inner (int i)
{
  return __builtin_dynamic_object_size (gr.ar[1].arr[i].name, 1);
}

int
main (void)
{
  EQ (n_int (1), 16);
  EQ (n_long (1), 16);
  EQ (n_short (1), 12);
  EQ (n_hdr (1), 16);
  EQ (n_row (1), 5);
  EQ (n_edge (1), 16);
  EQ (n_tag (1), 3);

  EQ (k_unsigned (1), 16);
  EQ (k_ptr (&g, 1), 16);
  EQ (k_plus (1), 16);

  GE (w_hdr (1), sizeof (g3) - 8);
  GE (w_row (1), sizeof (gm2) - 3);
  GE (w_inner (1), sizeof (gr) - 4);
  GE (w_outer (1, 1), sizeof (go) - 12);
  GE (w_edge (1), sizeof (g16));
  EQ (w_const (), sizeof (g3) - 8);

  EQ (d_int (2), 16);
  EQ (d_int (-1), 0);
  EQ (d_row (1), 5);
  EQ (d_row (-1), 0);
  EQ (d_hdr (1), 16);
  EQ (d_hdr (-1), sizeof (g3) - 8);
  EQ (d_inner (0), 16);
  EQ (d_inner (-1), sizeof (gr) - 24);
  return 0;
}
