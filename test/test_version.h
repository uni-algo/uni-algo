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

std::string processor_architecture()
{
    std::string result;
#if defined(__GNUC__) || defined(__clang__)
#  if defined(__i386__)
    result += " x86";
#  elif defined(__amd64__)
    result += " x64";
#  elif defined(__ia64__)
    result += " IA64";
#  elif defined(__aarch64__)
    result += " ARM64";
#  elif defined(__arm__)
    result += " ARM";
#  endif
#elif defined(_MSC_VER)
#  if defined(_M_IX86)
    result += " x86";
#  elif defined(_M_AMD64)
    result += " x64";
#  elif defined(_M_IA64)
    result += " IA64";
#  elif defined(_M_ARM64)
    result += " ARM64";
#  elif defined(_M_ARM)
    result += " ARM";
#  endif
#endif
    return result;
}

std::string compiler_clang_stdlib()
{
    // Find out what stdlib Clang uses
    std::string result;
#if defined(_LIBCPP_VERSION)
    result += " libc++ " + std::to_string(_LIBCPP_VERSION);
#elif defined(__GLIBCXX__)
#  if defined(_GLIBCXX_RELEASE)
    result += " libstdc++ " + std::to_string(_GLIBCXX_RELEASE);
#  else
    result += " libstdc++ " + std::to_string(__GLIBCXX__);
#  endif
#elif defined(_MSC_VER)
    result += " MSVC STL " + std::to_string(_MSC_VER / 100) + '.' + std::to_string(_MSC_VER % 100);
#endif
    return result;

    // NOTE:
    // Some info about Clang stdlib on Windows: https://blog.conan.io/2022/10/13/Different-flavors-Clang-compiler-Windows.html
    // Linux uses -stdlib=libstdc++ by default with ability to -stdlib=libc++
    // macOS (Xcode) AppleClang uses -stdlib=libc++ by default -stdlib=libstdc++ is deprecated
}

std::string compiler_gcc_stdlib()
{
    // Just in case output libstdc++ version for GCC too
    std::string result;
#if defined(__GLIBCXX__)
#  if defined(_GLIBCXX_RELEASE)
    result += " libstdc++ " + std::to_string(_GLIBCXX_RELEASE);
#  else
    result += " libstdc++ " + std::to_string(__GLIBCXX__);
#  endif
#endif
    return result;
}

std::string test_version_compiler()
{
    std::string result;

#if defined(__INTEL_LLVM_COMPILER) // icx
    result += "IntelLLVM " + std::to_string(__INTEL_LLVM_COMPILER) + processor_architecture();
#elif defined(__INTEL_COMPILER) // icc
    result += "Intel " + std::to_string(__INTEL_COMPILER) + processor_architecture();
#elif defined(__clang__)
#  if defined(__apple_build_version__)
    result += "Apple";
#  endif
#  if defined(__ARMCOMPILER_VERSION)
    result += "ARM";
#  endif
    result += "Clang " +
            std::to_string(__clang_major__) + '.' +
            std::to_string(__clang_minor__) + '.' +
            std::to_string(__clang_patchlevel__) + compiler_clang_stdlib() + processor_architecture();
#elif defined(__GNUC__)
#  if defined(__MINGW32__)
    result += "MinGW ";
#  endif
    result += "GCC " +
            std::to_string(__GNUC__) + '.' +
            std::to_string(__GNUC_MINOR__) + '.' +
            std::to_string(__GNUC_PATCHLEVEL__) + compiler_gcc_stdlib() + processor_architecture();
#elif defined(_MSC_VER)
    result += "MSVC " +
            std::to_string(_MSC_VER / 100) + '.' +
            std::to_string(_MSC_VER % 100) + processor_architecture();
#else
    result += "UNKNOWN";
#endif

#ifdef NDEBUG
    result += " Release";
#else
    result += " Debug";
#endif

    return result;
}
