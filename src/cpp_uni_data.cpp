/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#include <cstddef>
#include <type_traits>

#include "cpp_uni_config.h"

#ifndef UNI_ALGO_DISABLE_CASE
#include "impl/impl_case_data_extern.h"
#include "impl/impl_case_data.h"
#endif

#ifndef UNI_ALGO_DISABLE_NORM
#include "impl/impl_norm_data_extern.h"
#include "impl/impl_norm_data.h"
#endif

#ifndef UNI_ALGO_DISABLE_PROP
#include "impl/impl_prop_data_extern.h"
#include "impl/impl_prop_data.h"
#endif

#ifndef UNI_ALGO_DISABLE_BREAK_GRAPHEME
#include "impl/impl_break_grapheme_data_extern.h"
#include "impl/impl_break_grapheme_data.h"
#endif

#ifndef UNI_ALGO_DISABLE_BREAK_WORD
#include "impl/impl_break_word_data_extern.h"
#include "impl/impl_break_word_data.h"
#endif

// TODO: Leave system locale stuff here for now
#ifndef UNI_ALGO_DISABLE_SYSTEM_LOCALE

#include "cpp_uni_locale.h"
#ifndef _WIN32
#include <cstdlib>
#else
#include "windows.h"
#endif

namespace uni::detail {

uni::locale locale_syscall()
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
        return uni::locale{lang};
#else
#  if WINVER >= 0x0600
    wchar_t name[LOCALE_NAME_MAX_LENGTH] = {};
    if (LCIDToLocaleName(LOCALE_USER_DEFAULT, name, LOCALE_NAME_MAX_LENGTH, 0))
        return uni::locale{name};
#  else
    wchar_t name[16] = {};
    int size = GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, name, static_cast<int>(std::size(name)));
    if (size)
    {
        name[size - 1] = '-';
        if (GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, name + size, static_cast<int>(std::size(name)) - size))
            return uni::locale{name};
    }
#  endif
#endif
    return uni::locale{};
}

const locale& locale_system()
{
    // Cannot be data race here locale object init only once and never changes
    static locale loc{detail::locale_syscall()};
    return loc;
}

#ifdef UNI_ALGO_EXPERIMENTAL

const uni::locale& locale_thread_impl(bool reinit)
{
    // Cannot be data race here locale object is thread_local every thread has it's own
    thread_local bool init = false;
    thread_local locale loc;
    if (!init || reinit)
    {
        if (reinit)
            loc = uni::locale{detail::locale_syscall()};
        else
            loc = locale_system();
        init = true;
    }
    return loc;
}
const uni::locale& locale_thread()
{
    return locale_thread_impl(false);
}
void locale_thread_reinit()
{
    locale_thread_impl(true);
}

#endif // UNI_ALGO_EXPERIMENTAL

} // namespace uni::detail

#endif // UNI_ALGO_DISABLE_SYSTEM_LOCALE
