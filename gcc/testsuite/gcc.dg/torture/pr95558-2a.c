/* { dg-do compile } */

extern void set_v (void);

int
weak_const (void)
{
  set_v ();
  return 0;
}
