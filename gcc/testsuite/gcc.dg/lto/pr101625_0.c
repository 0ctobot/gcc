/* { dg-lto-do link } */
/* { dg-require-linker-plugin "" } */
/* { dg-lto-options { { -Os -flto -ffat-lto-objects -r -nostdlib -no-pie } } } */

/* An incremental link of fat objects optimizes the merged unit.  IPA ICF turns
   one of these functions into a wrapper of the other and the inliner must see
   an initialized mod/ref summary for that wrapper.  */

typedef enum { XXH_OK } XXH_errorcode;
struct XXH64_state_s {};
struct XXH32_state_s {};
void XXH_free (void *);

XXH_errorcode
XXH32_freeState (struct XXH32_state_s *statePtr)
{
  XXH_free (statePtr);
  return XXH_OK;
}

XXH_errorcode
XXH64_freeState (struct XXH64_state_s *statePtr)
{
  XXH_free (statePtr);
  return XXH_OK;
}
