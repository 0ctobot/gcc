/* Without return address signing, the return address is still read from
   X30's incoming value, which doesn't stop shrink-wrapping since the
   prologue doesn't write X30.  */
/* { dg-do compile } */
/* { dg-options "-O2 -march=armv8.3-a -mbranch-protection=none -fno-omit-frame-pointer -fdump-rtl-pro_and_epilogue" } */

struct skb { unsigned len, data_len, tail, end; unsigned char *head; };
void panic_ (struct skb *, unsigned, void *) __attribute__ ((noreturn, cold));
void warn_ (struct skb *, unsigned, void *);

/* The return address is only read on a cold, noreturn path.  */
void *
put (struct skb *s, unsigned len)
{
  unsigned char *t = s->head + s->tail;
  s->tail += len;
  s->len += len;
  if (__builtin_expect (s->tail > s->end, 0))
    panic_ (s, len, __builtin_return_address (0));
  return t;
}

/* The return address is only read on a cold path that returns.  */
void *
put2 (struct skb *s, unsigned len)
{
  unsigned char *t = s->head + s->tail;
  s->tail += len;
  if (__builtin_expect (s->tail > s->end, 0))
    warn_ (s, len, __builtin_return_address (0));
  return t;
}

/* { dg-final { scan-rtl-dump-times "Performing shrink-wrapping" 2 "pro_and_epilogue" } } */
/* { dg-final { scan-assembler-times {hint\t7 // xpaclri} 2 } } */
/* { dg-final { scan-assembler-not {\txpaci\t} } } */
/* { dg-final { scan-assembler-not {\tldr\tx[0-9]+, \[(?:x29|sp), 8\]\n} } } */
