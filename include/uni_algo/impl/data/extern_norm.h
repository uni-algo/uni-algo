// Unicode 15.0.0

UNI_ALGO_IMPL_NAMESPACE_BEGIN

extern const unsigned char  stage1_decomp_nfd[];
extern const unsigned short stage2_decomp_nfd[];
extern const type_codept    stage3_decomp_nfd[];
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
extern const unsigned char  stage1_decomp_nfkd[];
extern const unsigned short stage2_decomp_nfkd[];
extern const type_codept    stage3_decomp_nfkd[];
#endif
extern const unsigned char  stage1_comp_cp1[];
extern const unsigned short stage2_comp_cp1[];
extern const unsigned char  stage1_comp_cp2[];
extern const unsigned short stage2_comp_cp2[];
extern const type_codept    stage3_comp[][64];
extern const unsigned char  stage1_ccc_qc[];
extern const unsigned short stage2_ccc_qc[];

UNI_ALGO_IMPL_NAMESPACE_END
