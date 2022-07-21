/* C++ Standart Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef CPP_UNI_BREAK_WORD_H_UAIX
#define CPP_UNI_BREAK_WORD_H_UAIX

#ifdef UNI_ALGO_DISABLE_BREAK_WORD
#  error "Break Word module is disabled via define UNI_ALGO_DISABLE_BREAK_WORD"
#endif

#include <type_traits>
#include <cassert>

#include <uni/config.h>
#include <uni/version.h>

#include <uni/impl/break_word.h>

namespace uni::breaks::word {

template<class Iter, class Sent = Iter>
class utf8
{
private:
    Iter it_begin;
    Sent it_end;
    Iter it_pos;
    Iter it_next;
    detail::type_codept word_prop = 0;
    detail::type_codept next_word_prop = 0;

    detail::impl_break_word_state state{};

public:
    // Only forward iterator can be used with default Word Boundary Rules
    // TODO: add static_assert for this
    using iterator_category = std::forward_iterator_tag;
    using value_type        = void;
    using pointer           = void;
    using reference         = void;
    using difference_type   = typename std::iterator_traits<Iter>::difference_type;

    using is_random_access_iterator_or_lower =
        std::is_convertible<typename std::iterator_traits<Iter>::iterator_category, std::random_access_iterator_tag>;

    utf8() = default;
    explicit utf8(Iter begin, Sent end)
        : it_begin{begin}, it_end{end}, it_pos{begin}, it_next{begin}
    {
        detail::impl_break_word_state_reset(&state);
    }
    explicit utf8(Iter end)
        : utf8{end, end} {}
    bool word_on_left() const noexcept { return detail::impl_break_is_word(word_prop); }
    utf8& operator++()
    {
        while (it_next != it_end)
        {
            it_pos = it_next;
            word_prop = next_word_prop;
            detail::type_codept codepoint = 0;
            it_next = detail::inline_utf8_iter(it_next, it_end, &codepoint, detail::impl_iter_replacement);
            if (detail::inline_utf8_break_word(&state, codepoint, &next_word_prop, it_next, it_end))
                return *this;
        }

        if (it_next == it_end)
        {
            it_pos = it_next;
            word_prop = next_word_prop;
        }

        return *this;
    }
    utf8 operator++(int)
    {
        utf8 tmp = *this;
        operator++();
        return tmp;
    }
    template<class T = difference_type> typename std::enable_if_t<is_random_access_iterator_or_lower::value, T>
    friend operator-(const utf8& x, const utf8& y) { return x.it_pos - y.it_pos; }
    friend bool operator==(const utf8& x, const utf8& y) { return (x.it_pos == y.it_pos); }
    friend bool operator!=(const utf8& x, const utf8& y) { return (x.it_pos != y.it_pos); }
    template<class S> friend bool operator==(const utf8& x, const S&) { return (x.it_pos == x.it_end); }
    template<class S> friend bool operator!=(const utf8& x, const S&) { return (x.it_pos != x.it_end); }
    template<class S> friend bool operator==(const S&, const utf8& x) { return (x.it_pos == x.it_end); }
    template<class S> friend bool operator!=(const S&, const utf8& x) { return (x.it_pos != x.it_end); }
};

template<class Iter, class Sent = Iter>
class utf16
{
private:
    Iter it_begin;
    Sent it_end;
    Iter it_pos;
    Iter it_next;
    detail::type_codept word_prop = 0;
    detail::type_codept next_word_prop = 0;

    detail::impl_break_word_state state{};

public:
    // Only forward iterator can be used with default Word Boundary Rules.
    // TODO: where is static_assert for this?
    using iterator_category = std::forward_iterator_tag;
    using value_type        = void;
    using pointer           = void;
    using reference         = void;
    using difference_type   = typename std::iterator_traits<Iter>::difference_type;

    using is_random_access_iterator_or_lower =
        std::is_convertible<typename std::iterator_traits<Iter>::iterator_category, std::random_access_iterator_tag>;

    utf16() = default;
    explicit utf16(Iter begin, Sent end)
        : it_begin{begin}, it_end{end}, it_pos{begin}, it_next{begin}
    {
        detail::impl_break_word_state_reset(&state);
    }
    explicit utf16(Iter end)
        : utf16{end, end} {}
    bool word_on_left() const noexcept { return detail::impl_break_is_word(word_prop); }
    utf16& operator++()
    {
        while (it_next != it_end)
        {
            it_pos = it_next;
            word_prop = next_word_prop;
            detail::type_codept codepoint = 0;
            it_next = detail::inline_utf16_iter(it_next, it_end, &codepoint, detail::impl_iter_replacement);
            if (detail::inline_utf16_break_word(&state, codepoint, &next_word_prop, it_next, it_end))
                return *this;
        }

        if (it_next == it_end)
        {
            it_pos = it_next;
            word_prop = next_word_prop;
        }

        return *this;
    }
    utf16 operator++(int)
    {
        utf16 tmp = *this;
        operator++();
        return tmp;
    }
    template<class T = difference_type> typename std::enable_if_t<is_random_access_iterator_or_lower::value, T>
    operator-(const utf16& rhs) { return (it_pos - rhs.it_pos); }
    friend bool operator==(const utf16& x, const utf16& y) { return (x.it_pos == y.it_pos); }
    friend bool operator!=(const utf16& x, const utf16& y) { return (x.it_pos != y.it_pos); }
    template<class S> friend bool operator==(const utf16& x, const S&) { return (x.it_pos == x.it_end); }
    template<class S> friend bool operator!=(const utf16& x, const S&) { return (x.it_pos != x.it_end); }
    template<class S> friend bool operator==(const S&, const utf16& x) { return (x.it_pos == x.it_end); }
    template<class S> friend bool operator!=(const S&, const utf16& x) { return (x.it_pos != x.it_end); }
};

} // namespace uni::breaks::word

#endif // CPP_UNI_BREAK_WORD_H_UAIX

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
