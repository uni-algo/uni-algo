/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_INTERNAL_RANGES_TRANSLIT_H_UAIH
#define UNI_ALGO_INTERNAL_RANGES_TRANSLIT_H_UAIH

#include <array>
#include <string_view>
#include <functional>

#include "ranges_core.h"

namespace una {

namespace detail::translit {

// This class behaves the same as std::u32string
// but it never allocates so the size is static.
struct buffer
{
private:
    std::array<char32_t, 16> internal_array{};
    std::size_t internal_size = 0;

public:
    using value_type      = char32_t;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using iterator        = decltype(internal_array)::iterator; // value_type*;
    using const_iterator  = decltype(internal_array)::const_iterator; // const value_type*;

    static const size_type npos = static_cast<size_type>(-1);

    buffer() = default;
    constexpr explicit buffer(std::u32string_view view) // Use only for testing
    {
        if (view.size() <= internal_array.max_size())
        {
            for (std::size_t i = 0; i < view.size(); ++i)
                internal_array[i] = view[i];
            internal_size = view.size();
        }
    }
    constexpr reference operator[](size_type pos) noexcept
    { return internal_array[pos]; }
    constexpr const_reference operator[](size_type pos) const noexcept
    { return internal_array[pos]; }
    constexpr size_type size() const noexcept
    { return internal_size; }
    constexpr size_type max_size() const noexcept
    { return internal_array.max_size(); }
    constexpr bool empty() const noexcept
    { return internal_size == 0; }
    constexpr pointer data() noexcept
    { return internal_array.data(); }
    constexpr const_pointer data() const noexcept
    { return internal_array.data(); }
    constexpr iterator begin() noexcept
    { return internal_array.begin(); }
    constexpr const_iterator begin() const noexcept
    { return internal_array.begin(); }
    constexpr iterator end() noexcept
    { return internal_array.begin() + static_cast<std::ptrdiff_t>(internal_size); }
    constexpr const_iterator end() const noexcept
    { return internal_array.begin() + static_cast<std::ptrdiff_t>(internal_size); }
    constexpr reference front() noexcept
    { return internal_array[0]; }
    constexpr const_reference front() const noexcept
    { return internal_array[0]; }
    constexpr reference back() noexcept
    { return internal_array[internal_size - 1]; }
    constexpr const_reference back() const noexcept
    { return internal_array[internal_size - 1]; }
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    constexpr operator std::u32string_view() const noexcept
    { return std::u32string_view{data(), size()}; }
    constexpr void clear() noexcept
    { internal_size = 0; }
    constexpr void pop_back() noexcept
    { internal_size--; }
    constexpr void push_back(value_type c) noexcept
    {
        if (size() < max_size())
            internal_array[internal_size++] = c;
        // else std::u32string throws
    }
    constexpr void resize(size_type count, value_type c) noexcept
    {
        if (count > max_size())
            return; // std::u32string throws

        for (size_type i = size(); i < count; ++i)
            internal_array[i] = c;

        internal_size = count;
    }
    constexpr void resize(size_type count) noexcept { resize(count, 0); }
    constexpr buffer& erase(size_type pos = 0, size_type count = npos) noexcept
    {
        if (count == 0)
            return *this;
        if (pos > size())
            return *this; // std::u32string throws
        if (count > size() - pos)
            count = size() - pos;

        internal_size -= count;

        for (size_type i = pos; i < size(); ++i)
            internal_array[i] = internal_array[i + count];

        return *this;
    }
    constexpr buffer& insert(size_type pos, size_type count, value_type c) noexcept
    {
        if (count == 0)
            return *this;
        if (pos > size())
            return *this; // std::u32string throws
        if (count + size() > max_size())
            return *this; // std::u32string throws

        internal_size += count;

        for (size_type i = size() - 1; i >= pos + count; --i)
            internal_array[i] = internal_array[i - count];

        for (size_type i = pos; i < pos + count; ++i)
            internal_array[i] = c;

        return *this;
    }
    constexpr buffer& replace(size_type pos, size_type count1, size_type count2, value_type c) noexcept
    {
        if (count1 == 0 && count2 == 0)
            return *this;
        if (pos > size())
            return *this; // std::u32string throws
        if (count1 > size() - pos)
            count1 = size() - pos;
        if (count1 < count2 && (count2 - count1) + size() > max_size())
            return *this; // std::u32string throws

        if (count1 > count2)
        {
            const size_type count = count1 - count2;
            internal_size -= count;
            for (size_type i = pos; i < size(); ++i)
                internal_array[i] = internal_array[i + count];
        }
        else if (count1 < count2)
        {
            const size_type count = count2 - count1;
            internal_size += count;
            for (size_type i = size() - 1; i >= pos + count; --i)
                internal_array[i] = internal_array[i - count];
        }
        for (size_type i = pos; i < pos + count2; ++i)
            internal_array[i] = c;

        return *this;
    }
    // Use std::enable_if_t to fix the ambiguous overload with the previous replace function
    template <class ArrayLike>
    typename std::enable_if_t<!std::is_integral_v<ArrayLike>, buffer&>
    constexpr replace(size_type pos1, size_type count1, const ArrayLike& array,
                      size_type pos2, size_type count2 = npos) noexcept
    {
        if (count1 == 0 && count2 == 0)
            return *this;
        if (pos1 > size() || pos2 > array.size())
            return *this; // std::u32string throws
        if (count1 > size() - pos1)
            count1 = size() - pos1;
        if (count2 > array.size() - pos2)
            count2 = array.size() - pos2;
        if (count1 < count2 && (count2 - count1) + size() > max_size())
            return *this; // std::u32string throws

        if (count1 > count2)
        {
            const size_type count = count1 - count2;
            internal_size -= count;
            for (size_type i = pos1; i < size(); ++i)
                internal_array[i] = internal_array[i + count];
        }
        else if (count1 < count2)
        {
            const size_type count = count2 - count1;
            internal_size += count;
            for (size_type i = size() - 1; i >= pos1 + count; --i)
                internal_array[i] = internal_array[i - count];
        }
        for (size_type i = pos1, j = pos2; i < pos1 + count2; ++i, ++j)
            internal_array[i] = array[j];

        return *this;
    }
    template <class ArrayLike>
    typename std::enable_if_t<!std::is_integral_v<ArrayLike>, buffer&>
    constexpr replace(size_type pos, size_type count, const ArrayLike& array) noexcept
    {
        return replace(pos, count, array, 0, array.size());
    }
    friend constexpr bool operator==(const buffer& x, const buffer& y) noexcept
    {
        if (x.size() == y.size())
        {
            for (size_type i = 0; i < x.size(); ++i)
                if (x[i] != y[i]) return false;
            return true;
        }
        return false;
    }
    friend constexpr bool operator!=(const buffer& x, const buffer& y) noexcept { return !(x == y); }
};

} // namespace detail::translit

namespace detail::rng {

// See ext/translit/macedonian_to_latin_docs.h for an example how to use it.

template<class Range, class Func>
class translit_view : public detail::rng::view_base
{
private:
    template<class Iter, class Sent>
    class translit
    {
        // Translit view is internal so skip static assert. For a reference it looks like this:
        //static_assert(std::is_integral_v<detail::rng::iter_value_t<Iter>> &&
        //              sizeof(detail::rng::iter_value_t<Iter>) >= sizeof(char32_t),
        //              "translit view requires char32_t range");

    private:
        translit_view* parent = nullptr;
        Iter it_pos;

        bool stream_end = false;

        detail::translit::buffer data;
        std::size_t skip = 1;

        uaiw_constexpr void iter_func_translit()
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
        using difference_type   = detail::rng::iter_difference_t<Iter>;

        uaiw_constexpr translit() = default;
        uaiw_constexpr explicit translit(translit_view& p, Iter begin, Sent)
            : parent{std::addressof(p)}, it_pos{begin}
        {
            iter_func_translit();
        }
        uaiw_constexpr reference operator*() const noexcept { return data[0]; }
        uaiw_constexpr translit& operator++()
        {
            iter_func_translit();

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
        friend uaiw_constexpr bool operator==(const translit& x, una::sentinel_t) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(const translit& x, una::sentinel_t) { return !x.stream_end; }
        friend uaiw_constexpr bool operator==(una::sentinel_t, const translit& x) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(una::sentinel_t, const translit& x) { return !x.stream_end; }
    };

    using iter_t = detail::rng::iterator_t<Range>;
    using sent_t = detail::rng::sentinel_t<Range>;

    Range range = Range{};
    Func func_translit;
    std::size_t buf_size = 1;
    detail::rng::cache<translit<iter_t, sent_t>> cached_begin;

public:
    uaiw_constexpr translit_view() = default;
    uaiw_constexpr translit_view(Range r, Func fun, std::size_t size)
        : range{std::move(r)}, func_translit{std::move(fun)}, buf_size{size} {}
    //uaiw_constexpr Range base() const& { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin.has_value())
            return cached_begin.get_value();

        cached_begin.set_value(translit<iter_t, sent_t>{*this, std::begin(range), std::end(range)});

        return cached_begin.get_value();
    }
    uaiw_constexpr auto end()
    {
        return una::sentinel;
    }
};

template<class Range, class Func>
translit_view(Range&&, Func, std::size_t) -> translit_view<views::all_t<Range>, Func>;

} // namespace detail::rng

} // namespace una

#endif // UNI_ALGO_INTERNAL_RANGES_TRANSLIT_H_UAIH

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
