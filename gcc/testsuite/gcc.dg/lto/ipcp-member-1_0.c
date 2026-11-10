/* { dg-lto-do link } */
/* { dg-lto-options { { -O2 -flto -fdump-ipa-cp-details } } } */

/* Both callers of drain pass &gr.cgrp, so IPA-CP should specialize drain for
   that constant.  struct opaque is incomplete here and complete in the other
   unit.  free_lang_data makes the target of a pointer member incomplete so
   that the two units' copies of a structure merge, but it leaves arrays of
   pointers alone, so the two copies of struct root stay unshared and each
   unit's &gr.cgrp has its own FIELD_DECL for cgrp.  Those are still the same
   member and must give one IPA-CP value.  */

struct opaque;
struct inner { int x; long y; };
struct root { struct opaque *o[2]; struct inner cgrp; int n; };
extern struct root gr;
extern long drain (struct inner *c);
extern long use_b (void);

int
main (void)
{
  return (int) (drain (&gr.cgrp) + use_b ());
}

/* { dg-final { scan-wpa-ipa-dump "Creating a specialized node of drain" "cp" } } */
