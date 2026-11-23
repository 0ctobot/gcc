/* { dg-do run { target { le || be } } } */
/* { dg-options "-O2 -fno-strict-aliasing" } */

struct __attribute__((scalar_storage_order ("little-endian"))) little
{
  unsigned int value[2];
};

struct __attribute__((scalar_storage_order ("big-endian"))) big
{
  unsigned int value[2];
};

static __attribute__((noinline, noclone)) unsigned int
read_little (const struct little *p)
{
  return p->value[1];
}

static __attribute__((noinline, noclone)) unsigned int
read_big (const struct big *p)
{
  return p->value[1];
}

static __attribute__((noinline, noclone)) void
write_little (struct little *p, unsigned int x)
{
  p->value[1] = x;
}

static __attribute__((noinline, noclone)) void
write_big (struct big *p, unsigned int x)
{
  p->value[1] = x;
}

int
main (void)
{
  struct little l = { { 0, 42 } };
  struct big b = { { 0, 42 } };

  if (read_little (&l) != 42 || read_big (&b) != 42)
    __builtin_abort ();

  write_little (&l, 43);
  write_big (&b, 43);
  if (l.value[1] != 43 || b.value[1] != 43)
    __builtin_abort ();
  return 0;
}
