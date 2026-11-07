/* An index below the start of a member makes the same pointer as subtracting
   from the member's address, and that pointer may still be inside the enclosing
   object.  The size can't be less than what's left of that object from there.
   The same goes for an index below what a pointer to a structure points to,
   since that may be the middle of a bigger object.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

typedef __SIZE_TYPE__ size_t;

struct pkt { char tag[8]; char label[24]; unsigned w[3]; long z; };
struct pkt tbl[5];
char buf[64] __attribute__ ((aligned (16)));

#define TBL_END ((char *) (tbl + 5))
#define BUF_END (buf + sizeof (buf))
#define NOIPA __attribute__ ((noipa)) void
#define bos(p, t) __builtin_object_size (p, t)
#define bdos(p, t) __builtin_dynamic_object_size (p, t)

/* The size may be unknown, but it can't be less than what's left.  */
#define check_left(p, end) \
  do \
    { \
      size_t left = (end) - (p); \
      if (bos (p, 0) < left || bos (p, 1) < left \
	  || bdos (p, 0) < left || bdos (p, 1) < left) \
	__builtin_abort (); \
    } \
  while (0)

/* The size is known and that's all that's left.  */
#define check_exact(p, end) \
  do \
    { \
      size_t left = (end) - (p); \
      if (bos (p, 0) != left || bos (p, 1) != left \
	  || bdos (p, 0) != left || bdos (p, 1) != left) \
	__builtin_abort (); \
    } \
  while (0)

NOIPA
const_index (void)
{
  char *p = &tbl[1].label[-8];
  check_exact (p, TBL_END);
}

NOIPA
const_index_past_member (void)
{
  char *p = &tbl[1].label[-16];
  check_exact (p, TBL_END);
}

NOIPA
var_element (int i)
{
  char *p = &tbl[i].label[-8];
  check_left (p, TBL_END);
}

NOIPA
param_to_start (struct pkt *e)
{
  char *p = &e->label[-8];
  check_left (p, TBL_END);
}

NOIPA
param_below_start (struct pkt *e)
{
  char *p = &e->tag[-8];
  check_left (p, TBL_END);
}

NOIPA
known_to_start (void)
{
  struct pkt *e = (struct pkt *) (buf + 16);
  char *p = &e->label[-8];
  check_exact (p, BUF_END);
}

NOIPA
known_below_start (void)
{
  struct pkt *e = (struct pkt *) (buf + 16);
  char *p = &e->tag[-8];
  check_exact (p, BUF_END);
}

NOIPA
var_below_start (int n)
{
  char *pos = buf + 32;
  pos += n;
  struct pkt *e = (struct pkt *) pos;
  char *p = &e->tag[-8];
  check_left (p, BUF_END);
}

NOIPA
either_below_start (int k)
{
  struct pkt *e = (struct pkt *) (k ? buf + 16 : buf + 32);
  char *p = &e->tag[-8];
  check_left (p, BUF_END);
}

/* Going forward from the start of a member still goes by that member, and the
   start of the whole object is still as low as a pointer can get.  */
NOIPA
controls (void)
{
  char *p = &tbl[1].label[0];
  char *q = &tbl[1].label[4];
  char *r = &tbl[1].label[24];
  char *s = &tbl[0].tag[-8];

  if (bos (p, 1) != 24 || bdos (p, 1) != 24)
    __builtin_abort ();
  if (bos (q, 1) != 20 || bdos (q, 1) != 20)
    __builtin_abort ();
  if (bos (r, 1) != 0 || bdos (r, 1) != 0)
    __builtin_abort ();
  if (bos (s, 1) != 0 || bdos (s, 1) != 0)
    __builtin_abort ();
}

NOIPA
heap_below_start (void)
{
  struct pkt *e = __builtin_malloc (sizeof (buf));
  char *p = &e->tag[-8];

  if (bos (p, 1) != 0 || bdos (p, 1) != 0)
    __builtin_abort ();
  __builtin_free (e);
}

int
main (void)
{
  const_index ();
  const_index_past_member ();
  var_element (1);
  param_to_start (&tbl[1]);
  param_below_start (&tbl[1]);
  known_to_start ();
  known_below_start ();
  var_below_start (-16);
  either_below_start (1);
  controls ();
  heap_below_start ();
  return 0;
}
