/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#ifndef UNI_ALGO_VERSION_H_UAIH
#define UNI_ALGO_VERSION_H_UAIH

#include "impl/impl_cpp_lib_version.h"
#include "impl/impl_unicode_version.h"

namespace una::version {
namespace library {
    inline constexpr int patch = (UNI_ALGO_CPP_LIB_VERSION % 1000);
    inline constexpr int minor = (UNI_ALGO_CPP_LIB_VERSION / 1000 % 1000);
    inline constexpr int major = (UNI_ALGO_CPP_LIB_VERSION / 1000000);
}
namespace unicode {
    inline constexpr int update = (UNI_ALGO_UNICODE_VERSION % 1000);
    inline constexpr int minor = (UNI_ALGO_UNICODE_VERSION / 1000 % 1000);
    inline constexpr int major = (UNI_ALGO_UNICODE_VERSION / 1000000);
}}

#endif // UNI_ALGO_VERSION_H_UAIH
