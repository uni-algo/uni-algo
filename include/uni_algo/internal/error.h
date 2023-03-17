/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#ifndef UNI_ALGO_INTERNAL_ERROR_H_UAIH
#define UNI_ALGO_INTERNAL_ERROR_H_UAIH

#include <cassert>

#include "../config.h"

namespace una {

class error
{
public:
    enum class code : unsigned short {success = 0, ill_formed_utf};
public:
    constexpr error() noexcept = default;
    constexpr error(error::code ec) noexcept : error_code{ec} {}
    constexpr error(error::code ec, std::size_t p) noexcept : position{p}, error_code{ec} {}
    constexpr explicit operator bool() const noexcept { return error_code != error::code::success; }
    constexpr void reset() noexcept { error_code = error::code::success; position = detail::impl_npos; }
    constexpr bool has_pos() const noexcept { return position != detail::impl_npos; }
    constexpr std::size_t pos() const noexcept { assert(operator bool()); assert(has_pos()); return position; }
    constexpr error::code get_code() const noexcept { return error_code; }
private:
    std::size_t position = detail::impl_npos;
    error::code error_code = error::code::success;
};

} // namespace una

#endif // UNI_ALGO_INTERNAL_ERROR_H_UAIH
