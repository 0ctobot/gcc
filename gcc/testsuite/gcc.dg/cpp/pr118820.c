/* PR c/118820 */
/* { dg-do preprocess } */

#pragma redefine_extname _Pragma ("pack(bar)")

/* { dg-final { scan-file pr118820.i "#pragma redefine_extname _Pragma \\(\"pack\\(bar\\)\"\\)" } } */
/* { dg-final { scan-file-not pr118820.i "#pragma pack" } } */
