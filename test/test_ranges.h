/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// If it fails a compiler messed up the encoding of this file
// It the compiler is MSVC then /utf-8 command line option must be used
static_assert(U'㋡' == 0x32E1);

void test_ranges()
{
    TESTX("789" == (std::u32string_view{U"123789"} | uni::views::drop(3) | uni::ranges::to_utf8<std::string>()));
    TESTX("123" == (std::u32string_view{U"123789"} | uni::views::take(3) | uni::ranges::to_utf8<std::string>()));
    TESTX("987321" == (std::u32string_view{U"123789"} | uni::views::reverse | uni::ranges::to_utf8<std::string>()));
    TESTX("123" == (std::u32string_view{U"   123"} | uni::views::filter([](char32_t c) { return c != ' '; }) | uni::ranges::to_utf8<std::string>()));
    // See comments in cpp_uni_ranges -> uni::ranges::to_utf8 adaptor
    TESTX("789" == (std::u32string_view{U"123"} | uni::views::transform([](char32_t c) -> char32_t { return c + 6; }) | uni::ranges::to_utf8<std::string>()));
    TESTX("789" == (std::u32string_view{U"123"} | uni::views::transform([](char32_t c) { return c + 6; }) | uni::ranges::to_utf8<std::string>()));

    //std::string result = std::string_view{"12345678900"}
    std::string result = std::string_view{"アイウエオカキクケココ"}
             | uni::views::utf8
             | uni::views::reverse | uni::views::take(7) | uni::views::reverse
             | uni::views::drop(2) | uni::views::reverse
             | uni::ranges::to_utf8<std::string>();
    //TESTX{result == "00987"};
    TESTX(result == "ココケクキ");

    // All of the following must be equivalent

    auto range1 = std::string_view{"アイウエオカキクケココ"}
             | uni::views::utf8
             | uni::views::reverse | uni::views::take(7) | uni::views::reverse
             | uni::views::drop(2) | uni::views::reverse;
    std::u32string result1{range1.begin(), range1.end()};
    TESTX(result1 == U"ココケクキ");

    auto range2 = uni::views::reverse(
        uni::views::drop(
            uni::views::reverse(
                uni::views::take(
                    uni::views::reverse(
                        uni::views::utf8(std::string_view{"アイウエオカキクケココ"})
                ), 7)
        ), 2)
    );
    std::u32string result2{range2.begin(), range2.end()};
    TESTX(result2 == U"ココケクキ");

    auto range3 = uni::ranges::reverse_view{
        uni::ranges::drop_view{
            uni::ranges::reverse_view{
                uni::ranges::take_view{
                    uni::ranges::reverse_view{
                        uni::ranges::utf8_view{std::string_view{"アイウエオカキクケココ"}}
                }, 7}
        }, 2}
    };
    std::u32string result3{range3.begin(), range3.end()};
    TESTX(result3 == U"ココケクキ");
}

void test_ranges_ctad()
{
    // If everything works properly none of these constructors and operators must be called
    class my_string
    {
    private:
        std::string str;
    public:
        my_string() = default;
        explicit my_string(const char* s) { str = s; }
        my_string(const my_string& s) { TESTX(false); std::cout << "copy contruct" << '\n'; str = s.str; }
        my_string(my_string&& s) { TESTX(false); std::cout << "move contruct" << '\n'; str = std::move(s.str); }
        my_string& operator=(const my_string& s) { TESTX(false); std::cout << "copy operator" << '\n'; str = s.str; return *this; }
        my_string& operator=(my_string&& s) { TESTX(false); std::cout << "move operator" << '\n'; str = std::move(s.str); return *this; }
        auto begin() const { return str.begin(); }
        auto end() const { return str.end(); }
    };

    my_string str{"12345678900"};
    TESTX(*(str | uni::views::all).begin() == '1');
    TESTX(*(str | uni::views::utf8).begin() == U'1');
    TESTX(*(str | uni::views::reverse).begin() == '0');
    TESTX(*(str | uni::views::take(1)).begin() == '1');
    TESTX(*(str | uni::views::drop(1)).begin() == '2');
    TESTX(*(str | uni::views::filter([](char c) { return c != '1'; })).begin() == '2');
    TESTX(*(str | uni::views::transform([](char) { return '7'; })).begin() == '7');

    // It works this way too (it just breaks compilation)
    // so for views that won't work with std::string just the simple test
    TESTX(*(u"123" | uni::views::utf16).begin() == u'1');
}

void test_ranges_static_assert()
{
    static_assert(std::is_same_v<decltype("12345" | uni::views::reverse),
            decltype(uni::ranges::reverse_view{uni::ranges::ref_view{"00000"}})>); // must be ref_view here
    static_assert(std::is_same_v<decltype(std::string_view{"12345"} | uni::views::reverse),
            decltype(uni::ranges::reverse_view{std::string_view{}})>); // must not be ref_view here
#if !defined(__cpp_lib_ranges) || __cpp_lib_ranges > 202106L
    static_assert(std::is_same_v<decltype(std::string{"12345"} | uni::views::reverse),
            decltype(uni::ranges::reverse_view{uni::ranges::owning_view{std::string{}}})>); // must be owning_view here
#endif
    // std::decay_t TEST
    /*
        // std::decay_t TEST (see our all view in cpp_uni_ranges.h for details)
        // Test: std::is_base_of_v<detail::ranges_view_base, std::decay_t<R>>
        const std::string_view str_view{"12345"};
        const auto my_view = str_view | uni::views::reverse;
        my_view | uni::views::drop(1) = 0; // must not be ref_view here
        // Test: std::is_same_v<std::basic_string_view<base_iterator_v>, std::decay_t<R>>
        const std::string_view str_view{"12345"};
        str_view | uni::views::utf8 = 0; // must not be ref_view here
    */
    { // Test: std::is_base_of_v<detail::ranges_view_base, std::decay_t<R>>
        const std::string_view str_view{"12345"};
        auto my_view = str_view | uni::views::reverse;
        static_assert(std::is_same_v<decltype(my_view | uni::views::drop(1)),
                decltype(uni::ranges::drop_view{uni::ranges::reverse_view{std::string_view{}}, 1})>); // must not be ref_view here
    }
    { // Test: std::is_same_v<std::basic_string_view<base_iterator_v>, std::decay_t<R>>
        const std::string_view str_view{"12345"};
        static_assert(std::is_same_v<decltype(str_view | uni::views::reverse),
                decltype(uni::ranges::reverse_view{std::string_view{}})>); // must not be ref_view here
    }
    // Other tests
#if !defined(__cpp_lib_ranges) || __cpp_lib_ranges > 202106L
    static_assert(std::is_same_v<decltype(std::string{"12345"} | uni::views::reverse | uni::views::drop(1)),
            decltype(uni::ranges::drop_view{uni::ranges::reverse_view{uni::ranges::owning_view{std::string{}}}, 1})>); // must be owning_view here
#endif
    {
        std::string str{"12345"};
        static_assert(std::is_same_v<decltype(str | uni::views::reverse),
                decltype(uni::ranges::reverse_view{uni::ranges::ref_view{str}})>); // must be ref_view here
    }
    {
        const std::string str{"12345"};
        static_assert(std::is_same_v<decltype(str | uni::views::reverse),
                decltype(uni::ranges::reverse_view{uni::ranges::ref_view{str}})>); // must be ref_view here
    }
    {
        std::string_view str_view{"12345"};
        static_assert(std::is_same_v<decltype(str_view | uni::views::reverse),
                decltype(uni::ranges::reverse_view{str_view})>); // must not be ref_view here
    }
    {
        const std::string_view str_view{"12345"};
        static_assert(std::is_same_v<decltype(str_view | uni::views::reverse),
                decltype(uni::ranges::reverse_view{str_view})>); // must not be ref_view here
    }
}
