/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_INTERNAL_RANGES_CORE_H_UAIH
#define UNI_ALGO_INTERNAL_RANGES_CORE_H_UAIH

// In Clang with libstdc++ std::ranges implementation doesn't work properly
// but it defines __cpp_lib_ranges so always force our own implementation of ranges in this case.
// Works fine when Clang with libc++ or MSVC STL (TODO: Not sure about MSVC STL need to test)
// TODO: maybe move the define to config later so it will be easier to test in other compilers too
#if defined(__clang__) && defined(__GLIBCXX__)
#  ifndef UNI_ALGO_FORCE_CPP17_RANGES
#  define UNI_ALGO_FORCE_CPP17_RANGES
#  endif
#endif

#include <iterator>
#include <utility> // std::forward
#include <memory> // std::addressof
#include <cassert>
#ifdef UNI_ALGO_LOG_CPP_ITER
#include <iostream>
#endif

#include "../config.h"

#if defined(__cpp_lib_ranges) && !defined(UNI_ALGO_FORCE_CPP17_RANGES)
#include <ranges>
#else
#include <string_view>
#endif

namespace una {

struct sentinel_t {};
inline constexpr sentinel_t sentinel;

namespace detail::rng {

// Inheritance from std::ranges::view_base must be used for compatibility
// with std::ranges  when a std::view on the right side of operator|
// https://tristanbrindle.com/posts/rvalue-ranges-and-views
#if !defined(__cpp_lib_ranges) || defined(UNI_ALGO_FORCE_CPP17_RANGES)
struct view_base {};
#else
using view_base = std::ranges::view_base;
#endif

#if !defined(__cpp_lib_ranges) || defined(UNI_ALGO_FORCE_CPP17_RANGES)
template<class Iter>
using iter_value_t = typename std::iterator_traits<Iter>::value_type;
template<class Iter>
using iter_pointer_t = typename std::iterator_traits<Iter>::pointer;
template<class Iter>
using iter_reference_t = typename std::iterator_traits<Iter>::reference;
template<class Iter>
using iter_difference_t = typename std::iterator_traits<Iter>::difference_type;
#else
template<class Iter>
using iter_value_t = std::iter_value_t<Iter>;
template<class Iter>
using iter_pointer_t = std::add_pointer_t<std::iter_reference_t<Iter>>;
template<class Iter>
using iter_reference_t = std::iter_reference_t<Iter>;
template<class Iter>
using iter_difference_t = std::iter_difference_t<Iter>;
#endif

template<class Range>
using iterator_t = decltype(std::begin(std::declval<Range&>())); // std::ranges::iterator_t<Range>
template<class Range>
using sentinel_t = decltype(std::end(std::declval<Range&>())); // std::ranges::sentinel_t<Range>

template<class Range>
using range_value_t = iter_value_t<iterator_t<Range>>; // std::ranges::range_value_t<Range>

#if !defined(__cpp_lib_ranges) || defined(UNI_ALGO_FORCE_CPP17_RANGES)
template<class Iter>
using iter_tag = typename std::iterator_traits<Iter>::iterator_category;
#else
// Bidirectional as highest should be good enough for all use cases in Unicode
// Commented code below is an example of random access as highest
template<class Iter>
using iter_tag = typename std::conditional_t<std::bidirectional_iterator<Iter>,
    std::bidirectional_iterator_tag, std::conditional_t<std::forward_iterator<Iter>,
    std::forward_iterator_tag, std::input_iterator_tag>>;

//template<class Iter>
//using iter_tag = typename std::conditional_t<std::random_access_iterator<Iter>,
//    std::random_access_iterator_tag, std::conditional_t<std::bidirectional_iterator<Iter>,
//    std::bidirectional_iterator_tag, std::conditional_t<std::forward_iterator<Iter>,
//    std::forward_iterator_tag, std::input_iterator_tag>>>;
#endif

// "has_member" helpers that are mostly used to disable/enable functions

template <typename T, typename = void>
struct has_member_begin : std::false_type {};
template <typename T>
struct has_member_begin<T, std::void_t<decltype(std::begin(std::declval<T&>()))>> : std::true_type {};

template <typename T, typename = void>
struct has_member_allocate : std::false_type {};
template <typename T>
struct has_member_allocate<T, std::void_t<decltype(std::declval<T&>().allocate(0))>> : std::true_type {};

template <typename T, typename = void>
struct has_member_data : std::false_type {};
template <typename T>
struct has_member_data<T, std::void_t<decltype(std::data(std::declval<T&>()))>> : std::true_type {};

// "is" helpers

#if !defined(__cpp_lib_ranges) || defined(UNI_ALGO_FORCE_CPP17_RANGES)
template<class Iter>
using is_iter_bidi_or_better = std::conditional_t<
    std::is_convertible_v<typename std::iterator_traits<Iter>::iterator_category, std::bidirectional_iterator_tag>,
    std::true_type, std::false_type>;
template<class Range>
using is_range_contiguous = std::conditional_t<has_member_data<Range>::value, std::true_type, std::false_type>;
#else
template<class Iter>
using is_iter_bidi_or_better = std::conditional_t<std::bidirectional_iterator<Iter>, std::true_type, std::false_type>;
template<class Range>
using is_range_contiguous = std::conditional_t<std::ranges::contiguous_range<Range>, std::true_type, std::false_type>;
#endif

// In C++17 std::string_view doesn't have iterators pair constructor
// so we use this a bit ugly approach to make it work. It is only used in text segmentation ranges.
// This helper function requeries contiguous range, but no checks here must be checked where used.
#if !defined(__cpp_lib_ranges) || defined(UNI_ALGO_FORCE_CPP17_RANGES)
template<class StringViewResult, class Range, class Iter>
constexpr StringViewResult to_string_view(const Range& range, Iter it_begin, Iter it_pos)
{
    return StringViewResult{std::data(range) + (it_begin - std::begin(range)),
                            static_cast<std::size_t>(it_pos - it_begin)};
}
#else
template<class StringViewResult, class Range, class Iter>
constexpr StringViewResult to_string_view(const Range&, Iter it_begin, Iter it_pos)
{
    return StringViewResult{it_begin, it_pos};
}
#endif

// Non-propagating cache from C++23 Standard (26.7.4)
// TODO: std::optional here instead of value/bool would be better, but as I remember std::optional
// is not constexpr in old compilers where we need it to be. But our constexpr library already
// requires pretty fresh compilers so it can be fine, need to recheck it and use it if possible.
template<class T>
class cache {
private:
    T val;
    bool has_val = false;

public:
    cache() = default;
    ~cache() = default;
    constexpr cache(const cache&) noexcept {}
    constexpr cache(cache&& other) noexcept { other.has_val = false; }

    constexpr cache& operator=(const cache& other) noexcept
    {
        if (std::addressof(other) != this)
            val.has_val = false;
        return *this;
    }

    constexpr cache& operator=(cache&& other) noexcept
    {
        has_val = false;
        other.has_val = false;
        return *this;
    }

    constexpr bool has_value() const noexcept { return has_val; }

    // No need this, use get_value instead
    //constexpr T& operator*() noexcept { return val; }
    //constexpr const T& operator*() const noexcept { return val; }

    constexpr T& get_value() noexcept { return val; }
    constexpr const T& get_value() const noexcept { return val; }
    constexpr void set_value(const T& value) noexcept
    {
        val = value;
        has_val = true;
    }
};

} // namespace detail::rng

namespace ranges {

// For C++17 we implement very simple ref_view that will be used together with una::views::all_t/una::views::all
// It has the similar design as std::views::ref_view so in C++20 we just use that
#if !defined(__cpp_lib_ranges) || defined(UNI_ALGO_FORCE_CPP17_RANGES)
template<class Range>
class ref_view : public detail::rng::view_base
{
private:
    Range* range = nullptr;
public:
    uaiw_constexpr ref_view() = default;
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    uaiw_constexpr ref_view(Range& r) : range{std::addressof(r)} {}
    //uaiw_constexpr Range& base() const { return *range; }
    uaiw_constexpr auto begin() const { return std::begin(*range); }
    uaiw_constexpr auto end() const { return std::end(*range); }
    template<class T = void, class = std::enable_if_t<detail::rng::has_member_data<Range>::value, T>>
    uaiw_constexpr auto data() const { return std::data(*range); }
};
#else
template<class R>
using ref_view = std::ranges::ref_view<R>;
#endif

// std::owning_view is available in C++20 starting with __cpp_lib_ranges >= 202110L
// but we still enable our owning_view only in C++17 so in C++20 inside all view sometimes
// std::owning_view will be used and sometimes std::ranges::subrange it shouldn't create any problems.
#if !defined(__cpp_lib_ranges) || defined(UNI_ALGO_FORCE_CPP17_RANGES) || (__cpp_lib_ranges >= 202110L)
#if !defined(__cpp_lib_ranges) || defined(UNI_ALGO_FORCE_CPP17_RANGES)
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2415r2.html
template<class Range>
class owning_view : public detail::rng::view_base
{
private:
    Range range = Range{};
public:
    uaiw_constexpr owning_view() = default;
    uaiw_constexpr ~owning_view() = default;
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    uaiw_constexpr owning_view(Range&& r) : range{std::move(r)} {}
    //uaiw_constexpr Range& base() & noexcept { return range; }
    //uaiw_constexpr const Range& base() const & noexcept { return range; }
    //uaiw_constexpr Range&& base() && noexcept { return std::move(range); }
    //uaiw_constexpr const Range&& base() const && noexcept { return std::move(range); }
    uaiw_constexpr owning_view(owning_view&&) noexcept = default;
    uaiw_constexpr owning_view& operator=(owning_view&&) noexcept = default;
    uaiw_constexpr owning_view(const owning_view&) = default;
    uaiw_constexpr owning_view& operator=(const owning_view&) = default;
    uaiw_constexpr auto begin() { return std::begin(range); }
    uaiw_constexpr auto end() { return std::end(range); }
    uaiw_constexpr auto begin() const { return std::begin(range); }
    uaiw_constexpr auto end() const { return std::end(range); }
    template<class T = void, class = std::enable_if_t<detail::rng::has_member_data<Range>::value, T>>
    uaiw_constexpr auto data() { return std::data(range); }
    template<class T = void, class = std::enable_if_t<detail::rng::has_member_data<Range>::value, T>>
    uaiw_constexpr auto data() const { return std::data(range); }
};
#else
template<class R>
using owning_view = std::ranges::owning_view<R>;
#endif
#endif

} // namespace ranges

namespace detail::rng {

// For C++17 we implement very simply view adaptors that support operator|
// The design is based on this article:
// https://mariusbancila.ro/blog/2020/06/06/a-custom-cpp20-range-view/
// For adaptors without parameters adaptor_closure object from this article is enough so we just call them adaptor
// and for adaptors with parameters both adaptor and adaptor_closure objects are needed
// For C++20 we still use them because C++ Commitee just want people to suffer and custom adaptors still not available
// For C++23 where such adaptors will be available we want to use them
// They will be called: std::ranges::range_adaptor_closure
// https://stackoverflow.com/questions/71689137/what-is-the-best-way-to-drop-last-element-using-c20-ranges
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2387r3.html
// Our adaptors support this:
// auto range = str | una::views::utf8 | una::views::filter(f)
// But do not support this:
// auto range = una::views::utf8 | una::views::filter(f)

/* ALL_VIEW */

#if !defined(__cpp_lib_ranges) || defined(UNI_ALGO_FORCE_CPP17_RANGES)
struct adaptor_all
{
    //uaiw_constexpr adaptor_closure_all() {}
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    {
        // The design of all view is "pretty simple"
        // for known ranges (our ranges) or std::string_view we just std::forward them directly
        // for unknown ranges (std::string etc.) we use ref_view as a proxy
        // and for rvalues we use owning_view as a proxy
        // this is the similar design as std::ranges

        using range_v = detail::rng::range_value_t<R>;

        // Check if the range is our range or std::ranges::view
        // Note that the better check in C++20 will look like this:
        // if constexpr (std::ranges::view<std::decay_t<R>>)
        // but in C++20 it is just better to use all view/ref_view/owning_view provided by the standard library
        // std::decay_t is important here to handle some corner cases properly
        // see: test/test_ranges.h -> test_ranges_static_assert()
        if constexpr (std::is_base_of_v<detail::rng::view_base, std::decay_t<R>> ||
                //std::is_base_of_v<std::ranges::view_interface<std::decay_t<R>>, std::decay_t<R>> || // view_interface check
                std::is_same_v<std::basic_string_view<range_v>, std::decay_t<R>>)
            return std::forward<R>(r);
        else if constexpr (std::is_lvalue_reference_v<R>)
            return ranges::ref_view{std::forward<R>(r)};
        else
            return ranges::owning_view{std::forward<R>(r)};

        // view_interface check is needed because std::ranges::view_interface in not derived from std::view_base anymore
        // https://cplusplus.github.io/LWG/issue3549
        // but we don't use this crappy all view in C++20 so we don't care
    }
};
template <class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_all& a) { return a(std::forward<R>(r)); }
#endif

} // namespace detail::rng

namespace ranges::views {

// In C++17 use our simple all view that uses our simple ref_view/owning_view
// In C++20 use facilities provided by the standard library
#if !defined(__cpp_lib_ranges) || defined(UNI_ALGO_FORCE_CPP17_RANGES)
inline constexpr detail::rng::adaptor_all all;
template<class Range>
using all_t = decltype(all(std::declval<Range>()));
#else
inline constexpr auto all = std::views::all;
template<class Range>
using all_t = std::views::all_t<Range>;
#endif

} // namespace ranges::views

namespace views = ranges::views;

} // namespace una

#endif // UNI_ALGO_INTERNAL_RANGES_CORE_H_UAIH

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
