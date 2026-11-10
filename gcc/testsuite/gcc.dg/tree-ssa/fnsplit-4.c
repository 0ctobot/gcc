/* { dg-do run } */
/* { dg-options "-O2 -fno-optimize-sibling-calls" } */

/* put_obj has a cheap early exit followed by a large tail that records its
   caller.  If ipa-split moved the tail into a split part, the recorded
   address would point into put_obj itself for every caller.  A sibling
   call from the header to the split part would hide that, so sibling
   calls are disabled, as the Linux kernel does.  */

struct obj { long refs; long state; void *owner; };

void *last_ip;

__attribute__ ((noipa)) void
record (void *ip, struct obj *p)
{
  last_ip = ip;
}

__attribute__ ((noipa)) void
work (struct obj *p, int n)
{
}

void
put_obj (struct obj *p)
{
  if (!p)
    return;
  record (__builtin_return_address (0), p);
  work (p, 1);
  work (p, 2);
  p->state = 0;
  work (p, 3);
  p->owner = 0;
  work (p, 4);
  if (p->state == 5)
    work (p, 5);
  work (p + 1, 105);
  if (p->state == 6)
    work (p, 6);
  work (p + 1, 106);
  if (p->state == 7)
    work (p, 7);
  work (p + 1, 107);
  if (p->state == 8)
    work (p, 8);
  work (p + 1, 108);
  if (p->state == 9)
    work (p, 9);
  work (p + 1, 109);
  if (p->state == 10)
    work (p, 10);
  work (p + 1, 110);
  if (p->state == 11)
    work (p, 11);
  work (p + 1, 111);
  if (p->state == 12)
    work (p, 12);
  work (p + 1, 112);
}

__attribute__ ((noinline)) void
user1 (struct obj *p)
{
  put_obj (p);
}

__attribute__ ((noinline)) void
user2 (struct obj *p)
{
  put_obj (p);
  put_obj (p + 1);
}

void (*volatile fp) (struct obj *) = put_obj;
struct obj objs[4];

int
main (void)
{
  void *ip1, *ip2;

  user1 (objs);
  user2 (objs);

  /* Two calls to the out-of-line put_obj from different places must
     record different return addresses.  */
  fp (objs);
  ip1 = last_ip;
  fp (objs);
  ip2 = last_ip;
  if (ip1 == ip2)
    __builtin_abort ();
  return 0;
}
