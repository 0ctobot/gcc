/* { dg-do run } */
/* { dg-options "-O2" } */

/* An int index that a check, a loop bound or an unsigned division keeps from
   going negative can't point below its array, so a member reached through a
   pointer of unknown size keeps its own size.  An index that's only checked
   against the end of its array can still be negative, which leaves the size
   unknown.  */

typedef __SIZE_TYPE__ size_t;

struct s
{
  unsigned int h[3];
  unsigned char name[3][66];
  unsigned char data[32];
  /* Keep data from being a possible flexible array member.  */
  int tail;
};

__attribute__ ((noipa)) size_t
row_div (struct s *p, unsigned int len)
{
  int slot = (len - 10) / 4;
  if (slot >= 3)
    return 0;
  return __builtin_dynamic_object_size (p->name[slot], 1);
}

__attribute__ ((noipa)) size_t
row_checked (struct s *p, int slot)
{
  if (slot < 0 || slot >= 3)
    return 0;
  return __builtin_dynamic_object_size (p->name[slot], 1);
}

__attribute__ ((noipa)) size_t
data_checked (struct s *p, int i)
{
  if (i < 0 || i > 20)
    return 0;
  return __builtin_dynamic_object_size (&p->data[i], 1);
}

__attribute__ ((noipa)) size_t
data_offset (struct s *p, int i)
{
  if (i < 0 || i > 20)
    return 0;
  return __builtin_dynamic_object_size (&p->data[i + 9], 1);
}

__attribute__ ((noipa)) void
data_loop (struct s *p, int n, size_t *out)
{
  for (int i = 0; i < n; i++)
    out[i] = __builtin_dynamic_object_size (&p->data[i], 1);
}

__attribute__ ((noipa)) size_t
data_upper (struct s *p, int i)
{
  if (i > 20)
    return 0;
  return __builtin_dynamic_object_size (&p->data[i], 1);
}

int
main (void)
{
  struct s *p = __builtin_malloc (sizeof (struct s));
  size_t out[4];

  if (row_div (p, 10) != 198 || row_div (p, 14) != 132
      || row_div (p, 18) != 66)
    __builtin_abort ();
  if (row_checked (p, 0) != 198 || row_checked (p, 2) != 66)
    __builtin_abort ();
  if (data_checked (p, 0) != 32 || data_checked (p, 20) != 12)
    __builtin_abort ();
  if (data_offset (p, 0) != 23 || data_offset (p, 20) != 3)
    __builtin_abort ();
  data_loop (p, 4, out);
  if (out[0] != 32 || out[3] != 29)
    __builtin_abort ();
  /* &p->data[-10] is in p->name[2], with 48 bytes left in *p.  */
  if (data_upper (p, -10) < 48)
    __builtin_abort ();
  __builtin_free (p);
  return 0;
}
