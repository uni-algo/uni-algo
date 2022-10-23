// Unicode 15.0.0

UNI_ALGO_IMPL_NAMESPACE_BEGIN

UNI_ALGO_DLL extern const unsigned char  stage1_lower[];
UNI_ALGO_DLL extern const type_codept    stage2_lower[];
UNI_ALGO_DLL extern const unsigned char  stage1_upper[];
UNI_ALGO_DLL extern const type_codept    stage2_upper[];
#ifndef UNI_ALGO_DISABLE_BREAK_WORD
UNI_ALGO_DLL extern const unsigned char  stage1_title[];
UNI_ALGO_DLL extern const type_codept    stage2_title[];
#endif
UNI_ALGO_DLL extern const unsigned char  stage1_fold[];
UNI_ALGO_DLL extern const type_codept    stage2_fold[];
UNI_ALGO_DLL extern const unsigned short stage1_order[];
UNI_ALGO_DLL extern const unsigned short stage2_order[];
#ifndef UNI_ALGO_DISABLE_FULL_CASE
UNI_ALGO_DLL extern const unsigned char  stage1_special_upper[];
UNI_ALGO_DLL extern const unsigned char  stage2_special_upper[];
UNI_ALGO_DLL extern const unsigned short stage3_special_upper[][4];
#ifndef UNI_ALGO_DISABLE_BREAK_WORD
UNI_ALGO_DLL extern const unsigned char  stage1_special_title[];
UNI_ALGO_DLL extern const unsigned char  stage2_special_title[];
UNI_ALGO_DLL extern const unsigned short stage3_special_title[][4];
#endif
UNI_ALGO_DLL extern const unsigned char  stage1_special_fold[];
UNI_ALGO_DLL extern const unsigned char  stage2_special_fold[];
UNI_ALGO_DLL extern const unsigned short stage3_special_fold[][4];
#endif
UNI_ALGO_DLL extern const unsigned char  stage1_case_prop[];
UNI_ALGO_DLL extern const unsigned char  stage2_case_prop[];

UNI_ALGO_IMPL_NAMESPACE_END
