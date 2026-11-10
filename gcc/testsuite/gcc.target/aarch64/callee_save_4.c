/* { dg-do compile } */
/* { dg-options "-O2 -mtune=neoverse-n1" } */

/* Reduced from iterate_iovec() in the Linux kernel's lib/iov_iter.c.  The
   loop on the unlikely path calls copy_user() with done, to, part and p
   live across the call.  part and p are only live on that path, so a
   callee-saved register for either of them is priced at the unlikely path's
   frequency, which makes it cheaper than a store and a load around every
   call in the loop.  Separate shrink-wrapping saves only one of the two
   registers on the unlikely path, though.  The other one is saved in the
   prologue together with the register it pairs with in an STP, which takes
   no more instructions than saving that partner alone.  */

struct iov { char *base; unsigned long len; };
struct iter { unsigned char type; unsigned long off; struct iov *iov;
	      unsigned long count; char *ubuf; };
unsigned long copy_user (void *, const void *, unsigned long);

unsigned long
copy_iter (char *to, struct iter *it, unsigned long len)
{
  unsigned long off = it->off, done = 0;

  if (__builtin_expect (it->type == 0, 1))
    {
      unsigned long rem = copy_user (to, it->ubuf + off, len);
      done = len - rem;
      it->off = off + done;
    }
  else if (it->type == 1)
    {
      struct iov *p = it->iov;
      do
	{
	  unsigned long part = p->len - off;
	  if (part > len)
	    part = len;
	  if (part)
	    {
	      unsigned long rem = copy_user (to + done, p->base + off, part);
	      unsigned long c = part - rem;
	      done += c;
	      off += c;
	      len -= c;
	      if (rem)
		break;
	    }
	  if (off >= p->len)
	    {
	      p++;
	      off = 0;
	    }
	}
      while (len);
      it->off = off;
      it->iov = p;
    }
  it->count -= done;
  return done;
}

/* done and to are live on the likely path too, so a callee-saved register
   for them would be saved at entry.  They stay in call-clobbered registers
   and are stored before the call in the loop and loaded after it, which
   are the only stack accesses that don't save or restore a callee-saved
   register.  */
/* { dg-final { scan-assembler-times {\t(?:ld|st)[rp]\tx(?:1[0-8]|[0-9]), (?:x[0-9]+, )?\[sp, [0-9]+\]} 3 } } */
