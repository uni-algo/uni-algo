/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#ifndef UNI_ALGO_INTERNAL_SEARCH_H_UAIH
#define UNI_ALGO_INTERNAL_SEARCH_H_UAIH

#include <cassert>

#include "../config.h"

namespace una {

class search
{
public:
    constexpr search() noexcept = default;
    constexpr explicit search(std::size_t p, std::size_t e) noexcept : position{p}, end_position{e} {}
    constexpr explicit operator bool() const noexcept { return position != detail::impl_npos; }
    constexpr void reset() noexcept { position = detail::impl_npos; end_position = detail::impl_npos; }
    constexpr std::size_t pos() const noexcept { assert(operator bool()); return position; }
    constexpr std::size_t end_pos() const noexcept { assert(operator bool()); return end_position; }
private:
    std::size_t position = detail::impl_npos;
    std::size_t end_position = detail::impl_npos;
};

} // namespace una

#endif // UNI_ALGO_INTERNAL_SEARCH_H_UAIH
