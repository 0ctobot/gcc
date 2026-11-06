/* PR128002 */
/* { dg-do compile } */
/* { dg-require-effective-target trampolines } */
/* { dg-options "-O0 -fsanitize=address -fdump-tree-sanopt" } */

int
f (int x, int y)
{
  int *p = &x;
  int *q = &y;
  int get (void) { return x; }
  return *p + *q + get ();
}

/* The incoming value of x must be read from the parameter in order to initialize
   the frame field, not from the frame field itself.  */
/* { dg-final { scan-tree-dump-not "= FRAME\\.\[0-9\]+\\.x;" "sanopt" } } */
