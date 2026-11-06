/* PR middle-end/126468 */
/* { dg-do compile } */
/* { dg-require-effective-target int32 } */
/* { dg-additional-options "-std=gnu11 -w" } */

/* IPA-CP narrows the long constant to the type of the int parameter, which sets
   TREE_OVERFLOW without any wrapping flag in effect.  The mismatched call makes
   this compile only.  */

static int leaf ();

__attribute__((noinline)) static int
mid (long y)
{
  return leaf (y);
}

__attribute__((noinline)) static int
leaf (x)
     int x;
{
  return __builtin_clrsb (x);
}

int
caller (void)
{
  return mid (0x1ffffffffL);
}

/* The inliner narrows the constant the same way when the mismatched call is
   what gets inlined.  */

static inline int inl_leaf ();

int
inl_caller (void)
{
  return inl_leaf (0x1ffffffffL);
}

static inline int
inl_leaf (x)
     int x;
{
  return __builtin_clrsb (x);
}

/* The original test case, where undefined signed overflow in the loop gets
   final value replacement to produce the same kind of constant.  */

static signed char g6;
long long g8;
_Bool g19;
int g31;

void
f1 (int a0)
{
  _Bool c1;
  int v5;
  signed char v7;
lbl_entry:
  switch (v5)
    {
    case 2065449875: goto lbl_b9;
    case 4: goto lbl_sw_def5;
    }
lbl_sw_def5:
  switch (a0)
    {
    case 401185226: __builtin_abort ();
    case 405534928: goto lbl_sw26;
    default: goto lbl_entry;
    }
lbl_b9:
  if (c1) goto lbl_br17;
lbl_br17:
  switch (v7)
    {
    case 108: goto lbl_sw19;
    case 3: goto lbl_bf52;
    default: goto lbl_sw26;
    }
lbl_sw19:
  switch (a0)
    {
    case 1068819367: goto lbl_sw26;
    case 9: goto lbl_sw26;
    case 4: return;
    default: goto lbl_sw_def28;
    }
lbl_sw26:
  if (g19) goto lbl_sw_def5;
  goto lbl_br17;
lbl_sw_def28:
  v5 = a0;
  g8 = 0;
lbl_sw_def34:
  g8 = 60976676962555186LL + g8;
  c1 = g8 <= 0;
  g31 = __builtin_clrsbll (g8);
  if (c1) goto lbl_bf39;
  goto lbl_sw_def34;
lbl_bf39:
  v7 = g6;
  if (v7) goto lbl_br17;
  switch (a0)
    {
    case 1: goto lbl_br47;
    case 5: return;
    }
  goto lbl_dead84;
lbl_br47:
  c1 = 1;
  goto lbl_b9;
lbl_bf52:
  if (a0) return;
  if (c1) return;
  goto lbl_b9;
lbl_dead84:
  ;
}
