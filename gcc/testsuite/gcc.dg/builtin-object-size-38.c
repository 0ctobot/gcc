/* A variable index that's negative at run time takes a pointer below the
   start of the subobject it indexes, as a constant index below the start
   does, so a dynamic size goes by what's left in the enclosing object then.
   An index that stays inside the subobject keeps the subobject's size, and
   an enclosing object of unknown size leaves the size unknown rather than
   zero.  */
/* { dg-do run } */
/* { dg-options "-O2 -Wno-array-bounds" } */

typedef __SIZE_TYPE__ size_t;

struct pkt { char tag[8]; char label[24]; unsigned w[3]; long z; };

struct pkt tbl[5];
char buf[96] __attribute__ ((aligned (16)));

#define NOIPA __attribute__ ((noipa))
#define END ((char *) (tbl + 5))

NOIPA size_t
label_n (int n)
{
  return __builtin_dynamic_object_size (&tbl[1].label[n], 1);
}

NOIPA size_t
label_n8 (int n)
{
  return __builtin_dynamic_object_size (&tbl[1].label[n - 8], 1);
}

NOIPA size_t
label_in (int i, int n)
{
  return __builtin_dynamic_object_size (&tbl[i].label[n], 1);
}

NOIPA size_t
mem_label1 (int n)
{
  struct pkt *e = (struct pkt *) (buf + 16);
  return __builtin_dynamic_object_size (&e->label[n], 1);
}

NOIPA size_t
mem_label0 (int n)
{
  struct pkt *e = (struct pkt *) (buf + 16);
  return __builtin_dynamic_object_size (&e->label[n], 0);
}

NOIPA size_t
p_label (struct pkt *p, int n)
{
  return __builtin_dynamic_object_size (&p->label[n], 1);
}

NOIPA size_t
p_label_u (struct pkt *p, unsigned n)
{
  return __builtin_dynamic_object_size (&p->label[n], 1);
}

#define CHECK_LEFT(sz, p, end) \
  do { if ((sz) < (size_t) ((char *) (end) - (char *) (p))) \
	 __builtin_abort (); } while (0)

int
main (void)
{
  CHECK_LEFT (label_n (-8), &tbl[1].label[-8], END);
  CHECK_LEFT (label_n8 (0), &tbl[1].label[-8], END);
  CHECK_LEFT (label_in (2, -8), &tbl[2].label[-8], END);
  CHECK_LEFT (mem_label1 (-20), buf + 4, buf + 96);
  CHECK_LEFT (mem_label0 (-20), buf + 4, buf + 96);
  CHECK_LEFT (p_label (&tbl[1], -8), &tbl[1].label[-8], END);

  if (label_n (3) != 21 || label_n8 (10) != 22 || label_in (2, 5) != 19
      || mem_label1 (3) != 21 || mem_label0 (3) != 69
      || p_label_u (&tbl[1], 3) != 21)
    __builtin_abort ();

  return 0;
}
