/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#ifndef UNI_ALGO_VERSION_H_UAIH
#define UNI_ALGO_VERSION_H_UAIH

#include "impl/impl_cpp_lib_version.h"
#include "impl/impl_unicode_version.h"

namespace una::version {

class library_v
{
private:
    const unsigned short ver_major = static_cast<unsigned short>(UNI_ALGO_CPP_LIB_VERSION / 1000000);
    const unsigned char ver_minor = static_cast<unsigned char>(UNI_ALGO_CPP_LIB_VERSION / 1000 % 1000);
    const unsigned char ver_patch = static_cast<unsigned char>(UNI_ALGO_CPP_LIB_VERSION % 1000);
public:
    constexpr library_v() noexcept = default;
    constexpr library_v(int major, int minor, int patch) noexcept
        : ver_major{static_cast<unsigned short>(major)},
          ver_minor{static_cast<unsigned char>(minor)},
          ver_patch{static_cast<unsigned char>(patch)} {}
    constexpr int major() const noexcept { return ver_major; }
    constexpr int minor() const noexcept { return ver_minor; }
    constexpr int patch() const noexcept { return ver_patch; }
    friend constexpr bool operator==(const library_v& x, const library_v& y) noexcept
    {
        return x.major() == y.major() &&
               x.minor() == y.minor() &&
               x.patch() == y.patch();
    }
    friend constexpr bool operator!=(const library_v& x, const library_v& y) noexcept { return !(x == y); }
    friend constexpr bool operator<(const library_v& x, const library_v& y) noexcept
    {
        return x.major() != y.major() ? x.major() < y.major() :
               x.minor() != y.minor() ? x.minor() < y.minor() :
               x.patch() != y.patch() ? x.patch() < y.patch() : false;
    }
    friend constexpr bool operator>(const library_v& x, const library_v& y) noexcept { return y < x; }
    friend constexpr bool operator<=(const library_v& x, const library_v& y) noexcept { return !(x > y); }
    friend constexpr bool operator>=(const library_v& x, const library_v& y) noexcept { return !(x < y); }
};

class unicode_v
{
private:
    const unsigned short ver_major = static_cast<unsigned short>(UNI_ALGO_UNICODE_VERSION / 1000000);
    const unsigned char ver_minor = static_cast<unsigned char>(UNI_ALGO_UNICODE_VERSION / 1000 % 1000);
    const unsigned char ver_update = static_cast<unsigned char>(UNI_ALGO_UNICODE_VERSION % 1000);
public:
    constexpr unicode_v() noexcept = default;
    constexpr unicode_v(int major, int minor, int update) noexcept
        : ver_major{static_cast<unsigned short>(major)},
          ver_minor{static_cast<unsigned char>(minor)},
          ver_update{static_cast<unsigned char>(update)} {}
    constexpr int major() const noexcept { return ver_major; }
    constexpr int minor() const noexcept { return ver_minor; }
    constexpr int update() const noexcept { return ver_update; }
    friend constexpr bool operator==(const unicode_v& x, const unicode_v& y) noexcept
    {
        return x.major() == y.major() &&
               x.minor() == y.minor() &&
               x.update() == y.update();
    }
    friend constexpr bool operator!=(const unicode_v& x, const unicode_v& y) noexcept { return !(x == y); }
    friend constexpr bool operator<(const unicode_v& x, const unicode_v& y) noexcept
    {
        return x.major() != y.major() ? x.major() < y.major() :
               x.minor() != y.minor() ? x.minor() < y.minor() :
               x.update() != y.update() ? x.update() < y.update() : false;
    }
    friend constexpr bool operator>(const unicode_v& x, const unicode_v& y) noexcept { return y < x; }
    friend constexpr bool operator<=(const unicode_v& x, const unicode_v& y) noexcept { return !(x > y); }
    friend constexpr bool operator>=(const unicode_v& x, const unicode_v& y) noexcept { return !(x < y); }
};

inline constexpr library_v library; // Library version
inline constexpr unicode_v unicode; // Unicode version

} // namespace una::version

#endif // UNI_ALGO_VERSION_H_UAIH
