/* PR ipa/128092 */
/* { dg-do run } */
/* { dg-additional-options "-fno-ipa-cp-clone" } */

/* IPA bit-CP folded the MAX_EXPR and MIN_EXPR in these callers using an
   operand whose lattice mask had unknown bits above its precision, which made
   a known sign bit look varying and picked the wrong operand.  Cloning would
   give card_init a copy per size at -O3 and hide the bug there.  */

struct card
{
  void *private_data;
};

static struct card card0[2], card1[2], card2[2];

__attribute__ ((noipa)) static void
check (int ok)
{
  if (!ok)
    __builtin_abort ();
}

/* Reduced from snd_card_new and snd_card_init in the Linux kernel, where the
   store to private_data was dropped for the caller passing a nonzero size.  */

__attribute__ ((noinline)) static void
card_init (struct card *card, __SIZE_TYPE__ extra_size)
{
  card->private_data = 0;
  if (extra_size > 0)
    card->private_data = card + 1;
}

__attribute__ ((noinline)) static void
card_new (struct card *card, int extra_size)
{
  if (extra_size < 0)
    extra_size = 0;
  card_init (card, extra_size);
}

__attribute__ ((noinline)) static unsigned
max_callee (unsigned x)
{
  return x;
}

__attribute__ ((noinline)) static unsigned
max_caller (int x)
{
  return max_callee (x > 0 ? x : 0);
}

__attribute__ ((noinline)) static int
min_callee (int x)
{
  return x * 3;
}

__attribute__ ((noinline)) static int
min_caller (int x)
{
  return min_callee (x < 5 ? x : 5);
}

int
main (void)
{
  card_new (card0, 400);
  card_new (card1, 0);
  card_new (card2, 0);
  check (card0[0].private_data == &card0[1]);
  check (card1[0].private_data == 0);
  check (card2[0].private_data == 0);

  check (max_caller (1) + max_caller (0) == 1);

  check (min_caller (4) == 12);
  check (min_caller (6) == 15);

  return 0;
}
