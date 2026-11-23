/* { dg-do compile } */

extern int *sp;
extern int *hidden_alias;

void
ext (void)
{
  *sp = 2;
}

void
ext_hidden (void)
{
  *hidden_alias = 2;
}
