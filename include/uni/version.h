/* C++ Standart Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#ifndef CPP_UNI_VERSION_H_UAIX
#define CPP_UNI_VERSION_H_UAIX

#include <uni/impl/cpp_lib_version.h>
#include <uni/impl/unicode_version.h>

namespace uni::version {
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

#endif // CPP_UNI_VERSION_H_UAIX
