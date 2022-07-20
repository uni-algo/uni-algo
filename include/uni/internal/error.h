/* C++ Standart Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#ifndef CPP_UNI_ERROR_UAIX
#define CPP_UNI_ERROR_UAIX

#include <type_traits>
#include <cassert>

#include <uni/config.h>

namespace uni {

class error
{
public:
    error() noexcept = default;
    error(bool f, std::size_t p) noexcept : fail(f), position(p) {}
    explicit operator bool() const noexcept { return fail; }
    void reset() noexcept { fail = false; position = detail::impl_npos; }
    bool is_pos() const noexcept { return position != detail::impl_npos; }
    std::size_t pos() const noexcept { assert(fail); assert(is_pos()); return position; }
private:
    bool fail = false;
    std::size_t position = detail::impl_npos;
};

} // namespace uni

#endif // CPP_UNI_ERROR_UAIX
