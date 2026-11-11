/* { dg-do run } */

/* The IOR of single bit fields from the same word is 0 or 1, whatever
   STORE_FLAG_VALUE the target uses for the comparisons it may be
   simplified into.  */

typedef struct
{
  _Bool a : 1;
  _Bool b : 1;
  _Bool c : 1;
  _Bool d : 1;
  unsigned int e : 4;
} S;

__attribute__ ((noipa)) int
ior2 (S *s)
{
  return s->b | s->c;
}

__attribute__ ((noipa)) int
ior3 (S *s)
{
  return s->b | s->c | s->d;
}

int
main (void)
{
  S s = { 1, 0, 0, 0, 15 };

  if (ior2 (&s) != 0 || ior3 (&s) != 0)
    __builtin_abort ();
  s.b = 1;
  if (ior2 (&s) != 1 || ior3 (&s) != 1)
    __builtin_abort ();
  s.b = 0;
  s.c = 1;
  if (ior2 (&s) != 1 || ior3 (&s) != 1)
    __builtin_abort ();
  s.c = 0;
  s.d = 1;
  if (ior2 (&s) != 0 || ior3 (&s) != 1)
    __builtin_abort ();
  return 0;
}
