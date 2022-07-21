/* C++ Standart Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef CPP_UNI_ITERATOR_H_UAIX
#define CPP_UNI_ITERATOR_H_UAIX

#include <type_traits>
#include <cassert>
#ifdef UNI_ALGO_LOG_CPP_ITER
#include <iostream>
#endif

#include <uni/config.h>
#include <uni/version.h>
#include <uni/internal/sentinel.h>

#include <uni/impl/iterator.h>

namespace uni {
/*
namespace iter {
const char32_t replacement = detail::impl_iter_replacement;
const char32_t error = detail::impl_iter_error;
}
*/

namespace iter {

const char32_t error = detail::impl_iter_error;

template<class Iter, class Sent/* = Iter*/, char32_t Error = detail::impl_iter_replacement>
class utf8
{
private:
    Iter it_begin;
    Sent it_end;
    Iter it_pos;
    Iter it_next;
    detail::type_codept codepoint = 0;

    static_assert(Error == uni::iter::error || Error == detail::impl_iter_replacement);

public:
    // Make input iterator if Iter is input iterator,
    // forward iterator if Iter is forward iterator otherwise bidirectional iterator.
    // TODO: should use std::is_convertible here too probably.
    using is_input_iterator =
        std::is_same<typename std::iterator_traits<Iter>::iterator_category, std::input_iterator_tag>;
    using is_forward_iterator =
        std::is_same<typename std::iterator_traits<Iter>::iterator_category, std::forward_iterator_tag>;
    using iterator_category = std::conditional_t<is_input_iterator::value,
        std::input_iterator_tag, std::conditional_t<is_forward_iterator::value,
        std::forward_iterator_tag, std::bidirectional_iterator_tag>>;
    using value_type        = char32_t;
    using pointer           = void; // pointer operator->() is not implemented
    using reference         = char32_t;
    using difference_type   = typename std::iterator_traits<Iter>::difference_type;

    // This is used to disable functions.
    // Note that we use disable via the return type approach because
    // it gives better (more readable) template error messages in GCC and MSVC
    // and MSVS IntelliSense works properly only with this approach.
    using is_random_access_iterator_or_lower =
        std::is_convertible<typename std::iterator_traits<Iter>::iterator_category, std::random_access_iterator_tag>;
    using is_bidirectional_iterator_or_lower =
        std::is_convertible<typename std::iterator_traits<Iter>::iterator_category, std::bidirectional_iterator_tag>;

    utf8() = default;
    explicit utf8(Iter begin, Sent end, Iter pos)
        : it_begin{begin}, it_end{end}, it_pos{pos}, it_next{pos}
    {
        // Only begin or end positions are supported for now
        assert(it_pos == it_begin || it_pos == it_end);
        if (it_pos != it_end)
            it_pos = it_begin;
        if (it_begin != it_end && it_pos != it_end)
            it_next = detail::impl_utf8_iter(it_next, it_end, &codepoint, Error);
        // TODO: if pos is not begin or end we must skip forward to a boundary
        // probably max 3 code units? It's fine if we stop and return FFFD if there are
        // more than 3 ill-formed the data is ill-formed anyway in this case.
        // And we probably must return to the previous position (it_next = it_temp) in this case,
        // pos won't work with an input iterator anyway (it always will be begin or end) so it's fine.
        // The logic is not that simple so we need tests for this.
    }
    explicit utf8(Iter begin, Sent end)
        : utf8{begin, end, begin} {}
    explicit utf8(Iter end) :
        it_begin{end}, it_end{end}, it_pos{end}, it_next{end} {}
    reference operator*() const noexcept { return codepoint; }
    utf8& operator++()
    {
#ifdef UNI_ALGO_LOG_CPP_ITER
            std::cout << "uni::iter::utf8::operator++() " << this << '\n';
#endif
        it_pos = it_next;
        if (it_pos == it_end)
            return *this;
        it_next = detail::impl_utf8_iter(it_next, it_end, &codepoint, Error);
        return *this;
    }
    utf8 operator++(int)
    {
        utf8 tmp = *this;
        operator++();
        return tmp;
    }
    template<class T = utf8&> typename std::enable_if_t<is_bidirectional_iterator_or_lower::value, T>
    operator--()
    {
#ifdef UNI_ALGO_LOG_CPP_ITER
            std::cout << "uni::iter::utf8::operator--() " << this << '\n';
#endif
        it_next = it_pos;
        if (it_pos == it_begin)
            return *this;
        it_pos = detail::impl_utf8_iter_rev(it_begin, it_pos, &codepoint, Error);
        return *this;
    }
    template<class T = utf8> typename std::enable_if_t<is_bidirectional_iterator_or_lower::value, T>
    operator--(int)
    {
        utf8 tmp = *this;
        operator--();
        return tmp;
    }
    template<class T = difference_type> typename std::enable_if_t<is_random_access_iterator_or_lower::value, T>
    friend operator-(const utf8& x, const utf8& y) { return x.it_pos - y.it_pos; }
    friend bool operator==(const utf8& x, const utf8& y) { return x.it_pos == y.it_pos; }
    friend bool operator!=(const utf8& x, const utf8& y) { return x.it_pos != y.it_pos; }
    friend bool operator==(const utf8& x, const uni::sentinel_t&) { return x.it_pos == x.it_end; }
    friend bool operator!=(const utf8& x, const uni::sentinel_t&) { return x.it_pos != x.it_end; }
    friend bool operator==(const uni::sentinel_t&, const utf8& x) { return x.it_pos == x.it_end; }
    friend bool operator!=(const uni::sentinel_t&, const utf8& x) { return x.it_pos != x.it_end; }
};

template<class Iter, class Sent/* = Iter*/, char32_t Error = detail::impl_iter_replacement>
class utf16
{
private:
    Iter it_begin;
    Sent it_end;
    Iter it_pos;
    Iter it_next;
    detail::type_codept codepoint = 0;

    static_assert(Error == uni::iter::error || Error == detail::impl_iter_replacement);

    static_assert(sizeof(typename std::iterator_traits<Iter>::value_type) >= sizeof(char16_t));

public:
    using is_input_iterator =
        std::is_same<typename std::iterator_traits<Iter>::iterator_category, std::input_iterator_tag>;
    using is_forward_iterator =
        std::is_same<typename std::iterator_traits<Iter>::iterator_category, std::forward_iterator_tag>;
    using iterator_category = std::conditional_t<is_input_iterator::value,
        std::input_iterator_tag, std::conditional_t<is_forward_iterator::value,
        std::forward_iterator_tag, std::bidirectional_iterator_tag>>;
    using value_type        = char32_t;
    using pointer           = void; // pointer operator->() is not implemented
    using reference         = char32_t;
    using difference_type   = typename std::iterator_traits<Iter>::difference_type;

    using is_random_access_iterator_or_lower =
        std::is_convertible<typename std::iterator_traits<Iter>::iterator_category, std::random_access_iterator_tag>;
    using is_bidirectional_iterator_or_lower =
        std::is_convertible<typename std::iterator_traits<Iter>::iterator_category, std::bidirectional_iterator_tag>;

    utf16() = default;
    explicit utf16(Iter begin, Sent end, Iter pos)
        : it_begin{begin}, it_end{end}, it_pos{pos}, it_next{pos}
    {
        // Only begin or end positions are supported for now
        assert(it_pos == it_begin || it_pos == it_end);
        if (it_pos != it_end)
            it_pos = it_begin;
        if (it_begin != it_end && it_pos != it_end)
            it_next = detail::impl_utf16_iter(it_next, it_end, &codepoint, Error);
        // See TODO: in iter::utf8 but max 1 code unit.
    }
    explicit utf16(Iter begin, Sent end)
        : utf16{begin, end, begin} {}
    explicit utf16(Iter end) :
        it_begin{end}, it_end{end}, it_pos{end}, it_next{end} {}
    reference operator*() const noexcept { return codepoint; }
    utf16& operator++()
    {
#ifdef UNI_ALGO_LOG_CPP_ITER
            std::cout << "uni::iter::utf16::operator++() " << this << '\n';
#endif
        it_pos = it_next;
        if (it_pos == it_end)
            return *this;
        it_next = detail::impl_utf16_iter(it_next, it_end, &codepoint, Error);
        return *this;
    }
    utf16 operator++(int)
    {
        utf16 tmp = *this;
        operator++();
        return tmp;
    }
    template<class T = utf16&> typename std::enable_if_t<is_bidirectional_iterator_or_lower::value, T>
    operator--()
    {
#ifdef UNI_ALGO_LOG_CPP_ITER
            std::cout << "uni::iter::utf16::operator--() " << this << '\n';
#endif
        it_next = it_pos;
        if (it_pos == it_begin)
            return *this;
        it_pos = detail::impl_utf16_iter_rev(it_begin, it_pos, &codepoint, Error);
        return *this;
    }
    template<class T = utf16> typename std::enable_if_t<is_bidirectional_iterator_or_lower::value, T>
    operator--(int)
    {
        utf16 tmp = *this;
        operator--();
        return tmp;
    }
    template<class T = difference_type> typename std::enable_if_t<is_random_access_iterator_or_lower::value, T>
    friend operator-(const utf16& x, const utf16& y) { return x.it_pos - y.it_pos; }
    friend bool operator==(const utf16& x, const utf16& y) { return x.it_pos == y.it_pos; }
    friend bool operator!=(const utf16& x, const utf16& y) { return x.it_pos != y.it_pos; }
    friend bool operator==(const utf16& x, const uni::sentinel_t&) { return x.it_pos == x.it_end; }
    friend bool operator!=(const utf16& x, const uni::sentinel_t&) { return x.it_pos != x.it_end; }
    friend bool operator==(const uni::sentinel_t&, const utf16& x) { return x.it_pos == x.it_end; }
    friend bool operator!=(const uni::sentinel_t&, const utf16& x) { return x.it_pos != x.it_end; }
};

namespace output {

template<class Iter>
class utf8
{
private:
    Iter it_pos;

public:
    using iterator_category = std::output_iterator_tag;
    using value_type        = void;
    using pointer           = void;
    using reference         = void;
    using difference_type   = typename std::iterator_traits<Iter>::difference_type;

    using is_random_access_iterator_or_lower =
        std::is_convertible<typename std::iterator_traits<Iter>::iterator_category, std::random_access_iterator_tag>;

    utf8() = default;
    explicit utf8(Iter it) : it_pos{it} {}
    utf8& operator*() { return *this; } // no-op
    utf8& operator++() { return *this; } // no-op
    utf8& operator++(int) { return *this; } // no-op
    utf8& operator=(char32_t c)
    {
        it_pos = detail::impl_utf8_output(c, it_pos);
        return *this;
    }
    template<class T = difference_type> typename std::enable_if_t<is_random_access_iterator_or_lower::value, T>
    friend operator-(const utf8& x, const utf8& y) { return x.it_pos - y.it_pos; }
    // operator- can be used to determine how many code units operator= did output
};

template<class Iter>
class utf16
{
private:
    Iter it_pos;

public:
    using iterator_category = std::output_iterator_tag;
    using value_type        = void;
    using pointer           = void;
    using reference         = void;
    using difference_type   = typename std::iterator_traits<Iter>::difference_type;

    using is_random_access_iterator_or_lower =
        std::is_convertible<typename std::iterator_traits<Iter>::iterator_category, std::random_access_iterator_tag>;

    utf16() = default;
    explicit utf16(Iter it) : it_pos{it} {}
    utf16& operator*() { return *this; } // no-op
    utf16& operator++() { return *this; } // no-op
    utf16& operator++(int) { return *this; } // no-op
    utf16& operator=(char32_t c)
    {
        it_pos = detail::impl_utf16_output(c, it_pos);
        return *this;
    }
    template<class T = difference_type> typename std::enable_if_t<is_random_access_iterator_or_lower::value, T>
    friend operator-(const utf16& x, const utf16& y) { return x.it_pos - y.it_pos; }
    // operator- can be used to determine how many code units operator= did output
};

} // namespace output

namespace func {

template<class Func, class Iter, class Sent/* = Iter*/>
class filter
{
private:
    Iter it_begin;
    Sent it_end;
    Iter it_pos;
    Func func_filter;

    char32_t codepoint = 0;

    void func_filter_impl()
    {
        while(!func_filter(*it_pos) && ++it_pos != it_end);

        if (it_pos != it_end)
            codepoint = *it_pos;
    }

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = char32_t;
    using pointer           = void;
    using reference         = char32_t;
    using difference_type   = typename std::iterator_traits<Iter>::difference_type;

    filter() = default;
    explicit filter(Func func, Iter begin, Sent end)
        : it_begin{begin}, it_end{end}, it_pos{begin}, func_filter{func}
    {
        if (it_pos != it_end)
            func_filter_impl();
    }
    explicit filter(Func func, Iter end)
        : it_begin{end}, it_end{end}, it_pos{end}, func_filter{func} {}
    reference operator*() const noexcept { return codepoint; }
    filter& operator++()
    {
        if (it_pos != it_end && ++it_pos != it_end)
            func_filter_impl();
        return *this;
    }
    filter operator++(int)
    {
        filter tmp = *this;
        operator++();
        return tmp;
    }
    friend bool operator==(const filter& x, const filter& y) { return x.it_pos == y.it_pos; }
    friend bool operator!=(const filter& x, const filter& y) { return x.it_pos != y.it_pos; }
    friend bool operator==(const filter& x, const uni::sentinel_t&) { return x.it_pos == x.it_end; }
    friend bool operator!=(const filter& x, const uni::sentinel_t&) { return x.it_pos != x.it_end; }
    friend bool operator==(const uni::sentinel_t&, const filter& x) { return x.it_pos == x.it_end; }
    friend bool operator!=(const uni::sentinel_t&, const filter& x) { return x.it_pos != x.it_end; }
};

template<class Func, class Iter, class Sent/* = Iter*/>
class transform
{
private:
    Iter it_begin;
    Sent it_end;
    Iter it_pos;
    Func func_transform;

    char32_t codepoint = 0;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = char32_t;
    using pointer           = void;
    using reference         = char32_t;
    using difference_type   = typename std::iterator_traits<Iter>::difference_type;

    transform() = default;
    explicit transform(Func func, Iter begin, Sent end)
        : it_begin{begin}, it_end{end}, it_pos{begin}, func_transform{func}
    {
        if (it_pos != it_end)
            codepoint = func_transform(*it_pos);
    }
    explicit transform(Func func, Iter end)
        : it_begin{end}, it_end{end}, it_pos{end}, func_transform{func} {}
    reference operator*() const noexcept { return codepoint; }
    transform& operator++()
    {
        if (it_pos != it_end && ++it_pos != it_end)
            codepoint = func_transform(*it_pos);
        return *this;
    }
    transform operator++(int)
    {
        transform tmp = *this;
        operator++();
        return tmp;
    }
    friend bool operator==(const transform& x, const transform& y) { return x.it_pos == y.it_pos; }
    friend bool operator!=(const transform& x, const transform& y) { return x.it_pos != y.it_pos; }
    friend bool operator==(const transform& x, const uni::sentinel_t&) { return x.it_pos == x.it_end; }
    friend bool operator!=(const transform& x, const uni::sentinel_t&) { return x.it_pos != x.it_end; }
    friend bool operator==(const uni::sentinel_t&, const transform& x) { return x.it_pos == x.it_end; }
    friend bool operator!=(const uni::sentinel_t&, const transform& x) { return x.it_pos != x.it_end; }
};

} // namespace func

} // namespace iter

namespace detail::iter {

template<class Func, class Iter, class Sent/* = Iter*/>
class translit
{
private:
    Func func_translit;

    Iter it_begin;
    Sent it_end;
    Iter it_pos;

    bool stream_end = false;

    std::u32string data;
    std::size_t max_size = 1;
    std::size_t skip = 1;

    void func_translit_impl()
    {
        do
        {
            if (!data.empty() && skip)
                data.erase(0, 1);

            for (; data.size() < max_size && it_pos != it_end; ++it_pos)
                data.push_back(*it_pos);

            if (data.empty())
            {
                stream_end = true;
                return;
            }

            if (skip > 1)
                --skip;
            else
                skip = func_translit(data);
        }
        while (skip == 0);
    }

public:
    using iterator_category = std::input_iterator_tag;
    using value_type        = char32_t;
    using pointer           = void;
    using reference         = char32_t;
    using difference_type   = typename std::iterator_traits<Iter>::difference_type;

    translit() = default;
    explicit translit(Func func, std::size_t size, Iter begin, Sent end)
        : func_translit{func}, it_begin{begin}, it_end{end}, it_pos{begin}, max_size{size}
    {
        func_translit_impl();
    }
    explicit translit(Func func, Iter end)
        : func_translit{func}, it_begin{end}, it_end{end}, it_pos{end}, stream_end{true} {}
    reference operator*() const noexcept { return data[0]; }
    translit& operator++()
    {
        func_translit_impl();
        return *this;
    }
    translit operator++(int)
    {
        translit tmp = *this;
        operator++();
        return tmp;
    }
    friend bool operator==(const translit& x, const translit& y) { return x.stream_end == y.stream_end; }
    friend bool operator!=(const translit& x, const translit& y) { return x.stream_end != y.stream_end; }
    friend bool operator==(const translit& x, const uni::sentinel_t&) { return x.stream_end; }
    friend bool operator!=(const translit& x, const uni::sentinel_t&) { return !x.stream_end; }
    friend bool operator==(const uni::sentinel_t&, const translit& x) { return x.stream_end; }
    friend bool operator!=(const uni::sentinel_t&, const translit& x) { return !x.stream_end; }
};

} // namespace detail::iter::translit

} // namespace uni

#endif // CPP_UNI_ITERATOR_H_UAIX

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
