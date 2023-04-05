/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See LICENSE.txt */

// IMPORTANT: This file must never be used by a wrapper

// Must be included after impl_defines.h

#ifndef UNI_ALGO_IMPL_INTERNAL_STAGES_H_UAIH
#define UNI_ALGO_IMPL_INTERNAL_STAGES_H_UAIH

UNI_ALGO_IMPL_NAMESPACE_BEGIN

// This is the only function that directly access stage1 and stage2 tables.
// Note that some data can use stage3 table then stage2 result is index in stage3.
#ifdef __cplusplus
template<typename T1, typename T2>
uaix_always_inline_tmpl
uaix_static type_codept stages(type_codept c, const T1& stage1, const T2& stage2)
{
    // If c > 0x10FFFF the behaviour is undefined
    // For stage1/2_special_* tables if c > 0xFFFF the behaviour is undefined

#ifdef UNI_ALGO_TEST_ASSERT
    uaix_assert(c <= 0x10FFFF);
#endif

    return stage2[(stage1[c >> 7] << 7) + (c & 127)];
    // The same as: stage2[stage1[c / 128] * 128 + c % 128]
}
#else
// This is the only complex macro that is used in this library and only when compiled in C mode
// https://gcc.gnu.org/onlinedocs/cpp/Duplication-of-Side-Effects.html#Duplication-of-Side-Effects
#define stages(c, stage1, stage2) (stage2[(size_t)(stage1[(c) >> 7] << 7) + ((c) & 127)]) /* UNSAFE */
// REMINDER:
//#define stages(c, stage1, stage2) /*__extension__*/({ __typeof__(c) c_ = (c); (stage2[(size_t)(stage1[c_ >> 7] << 7) + (c_ & 127)]); })
#endif

UNI_ALGO_IMPL_NAMESPACE_END

#endif // UNI_ALGO_IMPL_INTERNAL_STAGES_H_UAIH
