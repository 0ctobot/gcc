/* A value that's zero extended to an offset can't be negative whatever it was
   before, so a pointer moved by it must keep the member's size.  */
/* { dg-do run { target lp64 } } */
/* { dg-options "-O2" } */

typedef __SIZE_TYPE__ size_t;

struct pkt { char tag[8]; char label[24]; unsigned w[3]; long z; };
struct pkt tbl[5];

#define L tbl[1].label
#define NOIPA __attribute__ ((noipa)) void
#define bos(p) __builtin_object_size (p, 1)
#define bdos(p) __builtin_dynamic_object_size (p, 1)

/* P only goes forward, so the size must remain that of the member.  */
#define check_member(p) \
  do \
    if (bos (p) > sizeof (L) || bdos (p) > sizeof (L)) \
      __builtin_abort (); \
  while (0)

#define FORWARD(name, args, ...) \
  NOIPA name args { __VA_ARGS__; check_member (p); }

FORWARD (zext_sub_alone, (unsigned a, unsigned b), char *p = L + (a - b))

int
main (void)
{
  zext_sub_alone (3, 1);
  return 0;
}
