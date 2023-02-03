/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

test_constexpr bool test_translit_buffer()
{
    // The internal translit buffer must behave the same as std::u32string

    una::detail::translit::buffer buffer{U"1234567890"};
    una::detail::translit::buffer buf = buffer;
    std::u32string buff32{U"1234567890"};
    std::u32string b32 = buff32;

    TESTX(buf.max_size() == 16);

    buf.resize(777);

    TESTX(buf.size() == 10);
    TESTX(b32.size() == 10);
    TESTX(!buf.empty());
    TESTX(!b32.empty());
    TESTX(buf.front() == U'1');
    TESTX(b32.front() == U'1');
    TESTX(buf.back() == U'0');
    TESTX(b32.back() == U'0');

    buf.push_back('!');
    b32.push_back('!');
    TESTX(buf.size() == 11);
    TESTX(b32.size() == 11);
    TESTX(buf == std::u32string_view{U"1234567890!"});
    TESTX(b32 == std::u32string_view{U"1234567890!"});
    buf.pop_back();
    b32.pop_back();
    TESTX(buf.size() == 10);
    TESTX(b32.size() == 10);
    TESTX(buf == std::u32string_view{U"1234567890"});
    TESTX(b32 == std::u32string_view{U"1234567890"});
    buf.resize(5);
    b32.resize(5);
    TESTX(buf.size() == 5);
    TESTX(b32.size() == 5);
    TESTX(buf == std::u32string_view{U"12345"});
    TESTX(b32 == std::u32string_view{U"12345"});
    buf.clear();
    b32.clear();
    TESTX(buf.empty());
    TESTX(b32.empty());
    TESTX(buf == std::u32string_view{U""});
    TESTX(b32 == std::u32string_view{U""});

    buf = buffer;
    b32 = buff32;
    buf.erase(3);
    b32.erase(3);
    TESTX(buf == std::u32string_view{U"123"});
    TESTX(b32 == std::u32string_view{U"123"});
    buf.erase(0);
    b32.erase(0);
    TESTX(buf == std::u32string_view{U""});
    TESTX(b32 == std::u32string_view{U""});
    buf = buffer;
    b32 = buff32;
    buf.erase(10);
    b32.erase(10);
    TESTX(buf == std::u32string_view{U"1234567890"});
    TESTX(b32 == std::u32string_view{U"1234567890"});
    buf.erase(3, 5);
    b32.erase(3, 5);
    TESTX(buf == std::u32string_view{U"12390"});
    TESTX(b32 == std::u32string_view{U"12390"});
    buf.erase(1, 100);
    b32.erase(1, 100);
    TESTX(buf == std::u32string_view{U"1"});
    TESTX(b32 == std::u32string_view{U"1"});
    // BAD VALUES
    buf.erase(0, 0);
    b32.erase(0, 0);
    TESTX(buf == std::u32string_view{U"1"});
    TESTX(b32 == std::u32string_view{U"1"});
    //buf.erase(100, 1); // This triggers -Warray-bounds false positive in GCC 12.1.0
    TESTX(buf == std::u32string_view{U"1"});

    buf.clear();
    b32.clear();
    buf.insert(0, 3, U'!');
    b32.insert(0, 3, U'!');
    TESTX(buf == std::u32string_view{U"!!!"});
    TESTX(b32 == std::u32string_view{U"!!!"});
    buf = buffer;
    b32 = buff32;
    buf.insert(10, 1, U'!');
    b32.insert(10, 1, U'!');
    TESTX(buf == std::u32string_view{U"1234567890!"});
    TESTX(b32 == std::u32string_view{U"1234567890!"});
    buf.insert(10, 2, U'!');
    b32.insert(10, 2, U'!');
    TESTX(buf == std::u32string_view{U"1234567890!!!"});
    TESTX(b32 == std::u32string_view{U"1234567890!!!"});
    buf = buffer;
    b32 = buff32;
    buf.insert(5, 3, U'!');
    b32.insert(5, 3, U'!');
    TESTX(buf == std::u32string_view{U"12345!!!67890"});
    TESTX(b32 == std::u32string_view{U"12345!!!67890"});
    buf.insert(0, 2, U'!');
    b32.insert(0, 2, U'!');
    TESTX(buf == std::u32string_view{U"!!12345!!!67890"});
    TESTX(b32 == std::u32string_view{U"!!12345!!!67890"});
    // BAD VALUES
    buf.insert(0, 0, U'!');
    b32.insert(0, 0, U'!');
    TESTX(buf == std::u32string_view{U"!!12345!!!67890"});
    TESTX(b32 == std::u32string_view{U"!!12345!!!67890"});
    buf.insert(100, 1, U'!');
    TESTX(buf == std::u32string_view{U"!!12345!!!67890"});

    buf.clear();
    b32.clear();
    buf.replace(0, 0, 3, U'!');
    b32.replace(0, 0, 3, U'!');
    TESTX(buf == std::u32string_view{U"!!!"});
    TESTX(b32 == std::u32string_view{U"!!!"});
    buf = buffer;
    b32 = buff32;
    buf.replace(0, 5, 1, U'!');
    b32.replace(0, 5, 1, U'!');
    TESTX(buf == std::u32string_view{U"!67890"});
    TESTX(b32 == std::u32string_view{U"!67890"});
    buf.replace(0, 1, 5, U'1');
    b32.replace(0, 1, 5, U'1');
    TESTX(buf == std::u32string_view{U"1111167890"});
    TESTX(b32 == std::u32string_view{U"1111167890"});
    buf.replace(0, 100, 0, U'0');
    b32.replace(0, 100, 0, U'0');
    TESTX(buf == std::u32string_view{U""});
    TESTX(b32 == std::u32string_view{U""});
    buf = buffer;
    b32 = buff32;
    buf.replace(5, 100, 1, U'!');
    b32.replace(5, 100, 1, U'!');
    TESTX(buf == std::u32string_view{U"12345!"});
    TESTX(b32 == std::u32string_view{U"12345!"});
    buf.replace(0, 0, 1, U'!');
    b32.replace(0, 0, 1, U'!');
    TESTX(buf == std::u32string_view{U"!12345!"});
    TESTX(b32 == std::u32string_view{U"!12345!"});
    // BAD VALUES
    buf.replace(0, 0, 0, U'!');
    b32.replace(0, 0, 0, U'!');
    TESTX(buf == std::u32string_view{U"!12345!"});
    TESTX(b32 == std::u32string_view{U"!12345!"});
    buf.replace(1, 0, 0, U'!');
    b32.replace(1, 0, 0, U'!');
    TESTX(buf == std::u32string_view{U"!12345!"});
    TESTX(b32 == std::u32string_view{U"!12345!"});

    std::array<unsigned char, 3> arr{{'A','B','C'}};

    buf = buffer;
    buf.replace(0, 2, arr);
    TESTX(buf == std::u32string_view{U"ABC34567890"});
    buf.replace(0, 5, arr, 1, 2);
    TESTX(buf == std::u32string_view{U"BC567890"});
    buf.replace(2, 3, arr, 2);
    TESTX(buf == std::u32string_view{U"BCC890"});
    buf.replace(3, 100, arr);
    TESTX(buf == std::u32string_view{U"BCCABC"});
    buf.replace(5, 100, arr, 0, 2);
    TESTX(buf == std::u32string_view{U"BCCABAB"});
    buf.replace(0, 2, arr, 0, 0);
    TESTX(buf == std::u32string_view{U"CABAB"});
    // BAD VALUES
    buf.replace(0, 0, arr, 0, 0);
    TESTX(buf == std::u32string_view{U"CABAB"});
    buf.replace(1, 0, arr, 0, 0);
    TESTX(buf == std::u32string_view{U"CABAB"});

    return true;
}
