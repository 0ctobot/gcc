/* A negative constant after a loop gives a pointer that the maximum of its
   chain of definitions doesn't follow, so going forward from there can't leave
   a size smaller than what's left in the member.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

struct pkt { char tag[8]; char label[24]; unsigned w[3]; long z; };
struct pkt tbl[5];

#define bos(p) __builtin_object_size (p, 1)
#define bdos(p) __builtin_dynamic_object_size (p, 1)

__attribute__ ((noipa)) void
back_after_loop (int k)
{
  char *p = tbl[1].label + 3;
  for (int j = 0; j < k; j++)
    p += 1;
  p += -5;
  p += 7;
  if (bos (p) < 19 || bdos (p) < 19)
    __builtin_abort ();
}

/* A loop that only goes forward keeps the size of the member.  */

__attribute__ ((noipa)) void
forward_loop (int k)
{
  char *p = tbl[1].label + 3;
  for (int j = 0; j < k; j++)
    p += 1;
  p += 7;
  if (bos (p) != 14)
    __builtin_abort ();
}

int
main ()
{
  back_after_loop (0);
  forward_loop (0);
  return 0;
}
