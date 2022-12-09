/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#ifndef UNI_ALGO_INTERNAL_ERROR_H_UAIH
#define UNI_ALGO_INTERNAL_ERROR_H_UAIH

#include <type_traits>
#include <cassert>

#include "../config.h"

namespace uni {

class error
{
public:
    error() noexcept = default;
    error(bool f, std::size_t p) noexcept : fail(f), position(p) {}
    explicit operator bool() const noexcept { return fail; }
    void reset() noexcept { fail = false; position = detail::impl_npos; }
    bool has_pos() const noexcept { return position != detail::impl_npos; }
    std::size_t pos() const noexcept { assert(fail); assert(has_pos()); return position; }
private:
    bool fail = false;
    std::size_t position = detail::impl_npos;
};

} // namespace uni

#endif // UNI_ALGO_INTERNAL_ERROR_H_UAIH
