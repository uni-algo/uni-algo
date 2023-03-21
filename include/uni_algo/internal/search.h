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
private:
    std::size_t found_pos = detail::impl_npos;
    std::size_t found_end_pos = detail::impl_npos;
public:
    constexpr search() noexcept = default;
    constexpr explicit search(std::size_t pos, std::size_t end_pos) noexcept : found_pos{pos}, found_end_pos{end_pos} {}
    constexpr explicit operator bool() const noexcept { return found_pos != detail::impl_npos; }
    constexpr void reset() noexcept { found_pos = detail::impl_npos; found_end_pos = detail::impl_npos; }
    constexpr std::size_t pos() const noexcept { assert(operator bool()); return found_pos; }
    constexpr std::size_t end_pos() const noexcept { assert(operator bool()); return found_end_pos; }
};

} // namespace una

#endif // UNI_ALGO_INTERNAL_SEARCH_H_UAIH
