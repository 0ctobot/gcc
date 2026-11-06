/* PR preprocessor/79516 */
/* { dg-do preprocess } */

#pragma message "Okay 1"
#if 0
#pragma message ("Not printed")
#endif

/* { dg-final { scan-file pr79516.i "#pragma message \"Okay 1\"" } } */
/* { dg-final { scan-file-not pr79516.i "Not printed" } } */
