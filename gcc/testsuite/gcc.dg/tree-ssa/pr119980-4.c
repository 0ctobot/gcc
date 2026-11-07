/* PR target/119980 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

/* Redundant stores with related alias sets still get removed when the sets
   involve a member with alias set zero.  */

struct inner { char c; };
struct outer { struct inner i; short s; };
struct both { char c; short s; };

/* The store to the char member is covered by the store to the whole struct.  */

void
f1 (struct both *p)
{
  *p = (struct both) { 0, 0 };
  p->c = 0;
}

/* The base set of the store is a child of the base set of the load.  */

void
f2 (struct outer *o)
{
  char v = o->i.c;
  ((struct inner *) o)->c = v;
}

/* { dg-final { scan-tree-dump-not "(->|\\.)c = " "optimized" } } */
