/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See LICENSE.txt */

UNI_ALGO_IMPL_NAMESPACE_BEGIN

extern const unsigned char  stage1_lower[];
extern const type_codept    stage2_lower[];
extern const unsigned char  stage1_upper[];
extern const type_codept    stage2_upper[];
#ifndef UNI_ALGO_DISABLE_BREAK_WORD
extern const unsigned char  stage1_title[];
extern const type_codept    stage2_title[];
#endif
extern const unsigned char  stage1_fold[];
extern const type_codept    stage2_fold[];
extern const unsigned short stage1_order[];
extern const unsigned short stage2_order[];
#ifndef UNI_ALGO_DISABLE_FULL_CASE
extern const unsigned char  stage1_special_upper[];
extern const unsigned char  stage2_special_upper[];
extern const unsigned short stage3_special_upper[][4];
#ifndef UNI_ALGO_DISABLE_BREAK_WORD
extern const unsigned char  stage1_special_title[];
extern const unsigned char  stage2_special_title[];
extern const unsigned short stage3_special_title[][4];
#endif
extern const unsigned char  stage1_special_fold[];
extern const unsigned char  stage2_special_fold[];
extern const unsigned short stage3_special_fold[][4];
#endif
extern const unsigned char  stage1_case_prop[];
extern const unsigned char  stage2_case_prop[];

UNI_ALGO_IMPL_NAMESPACE_END
