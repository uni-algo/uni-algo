/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// IMPORTANT: This file doesn't have include guards and must be inlined into .cpp file

#include "../locale.h"

#ifndef _WIN32
#include <cstdlib>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace una::detail {

una::locale locale_syscall()
{
    // Different threads may call this function at the same time
    // WINAPI is fine but POSIX std::getenv may be problematic
    // and nothing can be done with this.

#ifndef _WIN32
    // boost::locale tries to get default locale like this
    // so just follow the behaviour.
    // ICU does something similar but it tries
    // LC_MESSAGES instead of LC_CTYPE sometimes?
    // and the order is different: LC_ALL -> LC_CTYPE/LC_MESSAGES -> LANG
    const char* lang = nullptr;
    if (!lang || !*lang)
        lang = std::getenv("LC_CTYPE");
    if (!lang || !*lang)
        lang = std::getenv("LC_ALL");
    if (!lang || !*lang)
        lang = std::getenv("LANG");
    if (lang && *lang)
        return una::locale{lang};
#else
    // List of all possible locales on Windows: https://ss64.com/locale.html
#if WINVER >= 0x0600
    wchar_t name[LOCALE_NAME_MAX_LENGTH] = {};
    if (LCIDToLocaleName(LOCALE_USER_DEFAULT, name, LOCALE_NAME_MAX_LENGTH, 0))
        return una::locale{name};
#else
    // Maximum lenght for every GetLocaleInfoW call is nine including null char:
    // https://learn.microsoft.com/en-us/windows/win32/intl/locale-siso-constants
    wchar_t name[32] = {};
    int len = GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, name, static_cast<int>(std::size(name)));
    if (len)
    {
        name[len - 1] = L'-';
        if (GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, name + len, static_cast<int>(std::size(name)) - len))
            return una::locale{name};
    }
#endif // WINVER >= 0x0600
#endif // _WIN32
    return una::locale{};
}

UNI_ALGO_DLL const locale& locale_system()
{
    // Cannot be data race here locale object init only once and never changes
    static locale loc{detail::locale_syscall()};
    return loc;
}

#ifdef UNI_ALGO_EXPERIMENTAL

const una::locale& locale_thread_impl(bool reinit)
{
    // Cannot be data race here locale object is thread_local every thread has it's own
    thread_local bool init = false;
    thread_local locale loc;
    if (!init || reinit)
    {
        if (reinit)
            loc = una::locale{detail::locale_syscall()};
        else
            loc = locale_system();
        init = true;
    }
    return loc;
}

UNI_ALGO_DLL const una::locale& locale_thread()
{
    return locale_thread_impl(false);
}

UNI_ALGO_DLL void locale_thread_reinit()
{
    locale_thread_impl(true);
}

#endif // UNI_ALGO_EXPERIMENTAL

} // namespace una::detail
