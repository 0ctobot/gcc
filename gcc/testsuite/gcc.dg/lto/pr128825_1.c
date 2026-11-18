struct S { long pad; int y; };

void (*fp) (const int *);

void
call (const struct S *s)
{
  fp (&s->y);
}
