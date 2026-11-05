/* { dg-do compile } */
/* { dg-options "-O2 -fdump-rtl-ext_dce" } */

/* ext-dce removes the zero extension of V into C since only the low byte
   of C is ever stored.  Dropping every REG_EQUAL note in the function would
   also drop the notes recording that DD is 2 or 1 where it is copied from
   DIR.  combine needs those to see that DD fits in its two-bit field, and
   without them the bit-field store keeps an extra AND with -10.  */

struct hdr
{
  unsigned char cci, ehs;
  unsigned char reserved2 : 7, enable_crypto : 1;
  unsigned char interrupt : 1, data_direction : 2, reserved1 : 1,
    command_type : 4;
  unsigned int dunl;
};

struct lrb
{
  int command_type;
  int intr;
  int dir;
};

void g (int);

void
f (struct hdr *h, struct lrb *l, unsigned char *flags, unsigned char *q,
   int a, unsigned long v)
{
  int dd;
  int dir = l->dir;
  unsigned char c = 3;

  if (dir == 2)
    {
      dd = 2;
      *flags = 64;
    }
  else if (dir == 1)
    {
      dd = 1;
      *flags = 32;
    }
  else
    {
      dd = 0;
      *flags = 0;
    }
  h->command_type = l->command_type;
  h->data_direction = dd;
  if (a)
    {
      c = v;
      g (0);
    }
  q[1] = c;
  g (1);
}

/* { dg-final { scan-assembler-not {\tmov\tw[0-9]+, -10\n} } } */
/* { dg-final { scan-rtl-dump "Successfully transformed to:" "ext_dce" } } */
