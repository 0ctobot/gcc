/* A variable index into a flexible, [1] or [0] array that isn't the last
   member of its structure can reach the members that follow it, so a store
   through the array must not be taken to leave those members alone.  */
/* { dg-do run } */
/* { dg-options "-std=gnu11 -fno-strict-aliasing" } */

struct hdr { int n; char data[]; };
struct cmd { struct hdr h; char storage[16]; };

__attribute__((noipa)) int
midflex (struct cmd *c, int i)
{
  c->storage[0] = 1;
  c->h.data[i] = 2;	/* i == 0 is storage[0].  */
  return c->storage[0];
}

struct hdr1 { int n; int ids[1]; };
struct cmd1 { struct hdr1 h; int more_ids[3]; };

__attribute__((noipa)) int
mid_one (struct cmd1 *c, int i)
{
  c->more_ids[0] = 1;
  c->h.ids[i] = 2;	/* i == 1 is more_ids[0].  */
  return c->more_ids[0];
}

struct regs { int ctrl; int marker[0]; int a; int b; };

__attribute__((noipa)) int
marker (struct regs *r, int i)
{
  r->a = 1;
  r->marker[i] = 2;	/* i == 0 is a.  */
  return r->a;
}

struct tbl { struct hdr1 e[4]; };

__attribute__((noipa)) int
arr_of_hdr (struct tbl *t, int i)
{
  t->e[1].n = 1;
  t->e[0].ids[i] = 2;	/* i == 1 is e[1].n.  */
  return t->e[1].n;
}

int
main (void)
{
  struct cmd c = {};
  struct cmd1 c1 = {};
  struct regs r = {};
  struct tbl t = {};

  if (midflex (&c, 0) != 2 || mid_one (&c1, 1) != 2
      || marker (&r, 0) != 2 || arr_of_hdr (&t, 1) != 2)
    __builtin_abort ();
  return 0;
}
