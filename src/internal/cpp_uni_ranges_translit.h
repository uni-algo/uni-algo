/* C++ Standart Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef CPP_UNI_RANGES_TRANSLIT_H_UAIX
#define CPP_UNI_RANGES_TRANSLIT_H_UAIX

#include <string>
#include <functional>

#include "cpp_uni_ranges_core.h"

namespace uni { // NOLINT(modernize-concat-nested-namespaces)

namespace detail::ranges {

// See ext/translit/macedonian_to_latin_docs.h for an example how to use it.

template<class Range, class Func>
class translit_view : public detail::ranges::view_base
{
private:
    template<class Iter, class Sent>
    class translit
    {
        // Translit view is internal so skip static assert. For a reference it looks like this:
        //static_assert(std::is_integral_v<detail::ranges::iter_value_t<Iter>> &&
        //              sizeof(detail::ranges::iter_value_t<Iter>) >= sizeof(char32_t),
        //              "translit view requires char32_t range");

    private:
        translit_view* parent = nullptr;
        Iter it_pos;

        bool stream_end = false;

        std::u32string data;
        std::size_t skip = 1;

        void func_translit_impl()
        {
            do
            {
                if (!data.empty() && skip)
                    data.erase(0, 1);

                for (; data.size() < parent->buf_size && it_pos != std::end(parent->range); ++it_pos)
                    data.push_back(*it_pos);

                if (data.empty())
                {
                    stream_end = true;
                    return;
                }

                if (skip > 1)
                    --skip;
                else
                    skip = std::invoke(parent->func_translit, data);
            }
            while (skip == 0);
        }

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = char32_t;
        using pointer           = void;
        using reference         = char32_t;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

        uaiw_constexpr translit() = default;
        uaiw_constexpr explicit translit(translit_view& p, Iter begin, Sent)
            : parent{std::addressof(p)}, it_pos{begin}
        {
            func_translit_impl();
        }
        uaiw_constexpr reference operator*() const noexcept { return data[0]; }
        uaiw_constexpr translit& operator++()
        {
            func_translit_impl();

            return *this;
        }
        uaiw_constexpr translit operator++(int)
        {
            translit tmp = *this;
            operator++();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const translit& x, const translit& y) { return x.stream_end == y.stream_end; }
        friend uaiw_constexpr bool operator!=(const translit& x, const translit& y) { return x.stream_end != y.stream_end; }
        friend uaiw_constexpr bool operator==(const translit& x, uni::sentinel_t) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(const translit& x, uni::sentinel_t) { return !x.stream_end; }
        friend uaiw_constexpr bool operator==(uni::sentinel_t, const translit& x) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(uni::sentinel_t, const translit& x) { return !x.stream_end; }
    };

    using iter_t = detail::ranges::iterator_t<Range>;
    using sent_t = detail::ranges::sentinel_t<Range>;

    Range range = Range{};
    Func func_translit;
    std::size_t buf_size = 1;
    translit<iter_t, sent_t> cached_begin_value;
    bool cached_begin = false;

public:
    uaiw_constexpr translit_view() = default;
    uaiw_constexpr translit_view(Range r, Func fun, std::size_t size)
        : range{std::move(r)}, func_translit{std::move(fun)}, buf_size{size} {}
    //uaiw_constexpr Range base() const& { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = translit<iter_t, sent_t>{*this, std::begin(range), std::end(range)};
        cached_begin = true;

        return cached_begin_value;
    }
    uaiw_constexpr auto end()
    {
        return uni::sentinel;
    }
};

template<class Range, class Func>
translit_view(Range&&, Func, std::size_t) -> translit_view<uni::views::all_t<Range>, Func>;

} // namespace detail::ranges

} // namespace uni

#endif // CPP_UNI_RANGES_TRANSLIT_H_UAIX

/* Public Domain License
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
 * software, either in source code form or as a compiled binary, for any purpose,
 * commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors of this
 * software dedicate any and all copyright interest in the software to the public
 * domain. We make this dedication for the benefit of the public at large and to
 * the detriment of our heirs and successors. We intend this dedication to be an
 * overt act of relinquishment in perpetuity of all present and future rights to
 * this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
