/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

std::string test_version_cpp()
{
#ifdef _MSVC_LANG
    return std::to_string(_MSVC_LANG);
#else
    return std::to_string(__cplusplus);
#endif
}

std::string test_version_ranges()
{
    std::string result;
#ifdef __cpp_lib_ranges
    result += std::to_string(__cpp_lib_ranges);
#ifdef UNI_ALGO_FORCE_CPP17_RANGES
    result += " INTERNAL";
#endif
#else
    result += "INTERNAL";
#endif
    return result;
}

std::string test_version_compiler()
{
    std::string result;
#if defined(__clang__)
    result += "Clang " +
            std::to_string(__clang_major__) + '.' +
            std::to_string(__clang_minor__) + '.' +
            std::to_string(__clang_patchlevel__);
#elif defined(__GNUC__)
#if defined(__MINGW32__)
    result += "MinGW ";
#endif
    result += "GCC " +
            std::to_string(__GNUC__) + '.' +
            std::to_string(__GNUC_MINOR__) + '.' +
            std::to_string(__GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
    result += "MSVC " +
            std::to_string(_MSC_VER / 100) + '.' +
            std::to_string(_MSC_VER % 100);
#else
    result += "UNKNOWN";
#endif
    return result;
}
