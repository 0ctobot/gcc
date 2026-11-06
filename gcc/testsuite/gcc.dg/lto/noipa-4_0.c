/* { dg-lto-do run } */
/* { dg-lto-options { { -O2 -flto } } } */

/* The table holds the noipa declaration of z, so a call through the table must
   not see the body in the other TU once it's devirtualized.  */

extern void abort (void);

__attribute__ ((noipa)) int z (int);

struct ops
{
  int (*op) (int);
};

static const struct ops o = { .op = z };

int
main (void)
{
  int x = o.op (5);
  if (__builtin_constant_p (x))
    abort ();
  return x != 6;
}
