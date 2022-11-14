/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#ifndef UNI_ALGO_INTERNAL_SEARCH_UAIH
#define UNI_ALGO_INTERNAL_SEARCH_UAIH

#include <type_traits>
#include <cassert>

#include "../config.h"

namespace uni {

class search
{
public:
    search() noexcept = default;
    explicit search(bool f, std::size_t p, std::size_t e) noexcept
        : found(f), position(p), end_position(e) {}
    explicit operator bool() const noexcept { return found; }
    void reset() { found = false; position = detail::impl_npos; end_position = detail::impl_npos; }
    std::size_t pos() const noexcept { assert(found); return position; }
    std::size_t end_pos() const noexcept { assert(found); return end_position; }
private:
    bool found = false;
    std::size_t position = detail::impl_npos;
    std::size_t end_position = detail::impl_npos;
};

} // namespace uni

#endif // UNI_ALGO_INTERNAL_SEARCH_UAIH
