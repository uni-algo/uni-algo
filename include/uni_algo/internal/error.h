/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#ifndef UNI_ALGO_INTERNAL_ERROR_H_UAIH
#define UNI_ALGO_INTERNAL_ERROR_H_UAIH

#include <cassert>
#ifdef UNI_ALGO_EXPERIMENTAL
#include <string_view>
#endif

#include "../config.h"

namespace una {

class error
{
public:
    enum class code : unsigned short
    {
        success = 0,
        ill_formed_utf = 1,
        not_normalized = 2,
        not_stream_safe = 3,
    };
private:
    std::size_t error_pos = detail::impl_npos;
    error::code error_code = error::code::success;
public:
    constexpr error() noexcept = default;
    constexpr explicit error(error::code ec) noexcept : error_code{ec} {}
    constexpr explicit error(error::code ec, std::size_t pos) noexcept : error_pos{pos}, error_code{ec} {}
    constexpr explicit operator bool() const noexcept { return error_code != error::code::success; }
    constexpr void reset() noexcept { error_code = error::code::success; error_pos = detail::impl_npos; }
    constexpr bool has_pos() const noexcept { return error_pos != detail::impl_npos; }
    constexpr std::size_t pos() const noexcept { assert(operator bool()); assert(has_pos()); return error_pos; }
    constexpr error::code get_code() const noexcept { return error_code; }
    friend constexpr bool operator==(const error& x, const error::code& y) noexcept { return x.error_code == y; }
    friend constexpr bool operator!=(const error& x, const error::code& y) noexcept { return x.error_code != y; }
    friend constexpr bool operator==(const error::code& x, const error& y) noexcept { return x == y.error_code; }
    friend constexpr bool operator!=(const error::code& x, const error& y) noexcept { return x != y.error_code; }
#ifdef UNI_ALGO_EXPERIMENTAL
    constexpr std::string_view to_string_view() const noexcept
    {
        switch(error_code)
        {
            case code::success: return "success";
            case code::ill_formed_utf: return "ill-formed UTF";
            case code::not_normalized: return "not normalized";
            case code::not_stream_safe: return "not stream-safe";
            default: return "invalid error code";
        }
    }
#endif // UNI_ALGO_EXPERIMENTAL
};

} // namespace una

#endif // UNI_ALGO_INTERNAL_ERROR_H_UAIH
