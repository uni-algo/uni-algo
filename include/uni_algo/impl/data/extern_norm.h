// Unicode 15.0.0

UNI_ALGO_IMPL_NAMESPACE_BEGIN

UNI_ALGO_DLL extern const unsigned char  stage1_decomp_nfd[];
UNI_ALGO_DLL extern const unsigned short stage2_decomp_nfd[];
UNI_ALGO_DLL extern const type_codept    stage3_decomp_nfd[];
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
UNI_ALGO_DLL extern const unsigned char  stage1_decomp_nfkd[];
UNI_ALGO_DLL extern const unsigned short stage2_decomp_nfkd[];
UNI_ALGO_DLL extern const type_codept    stage3_decomp_nfkd[];
#endif
UNI_ALGO_DLL extern const unsigned char  stage1_comp_cp1[];
UNI_ALGO_DLL extern const unsigned short stage2_comp_cp1[];
UNI_ALGO_DLL extern const unsigned char  stage1_comp_cp2[];
UNI_ALGO_DLL extern const unsigned short stage2_comp_cp2[];
UNI_ALGO_DLL extern const type_codept    stage3_comp[][64];
UNI_ALGO_DLL extern const unsigned char  stage1_ccc_qc[];
UNI_ALGO_DLL extern const unsigned short stage2_ccc_qc[];

UNI_ALGO_IMPL_NAMESPACE_END
