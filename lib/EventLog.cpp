/**
  @brief Event log utility class
  @author Uriel Mann
  @copyright 2023 Uriel Mann (abba.mann@gmail.com)

:: Permission is hereby granted, free of charge, to any person obtaining a copy
:: of this software and associated documentation files (the "Software"), to deal
:: in the Software without restriction, including without limitation the rights
:: to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
:: copies of the Software, and to permit persons to whom the Software is
:: furnished to do so, subject to the following conditions:

:: The above copyright notice and this permission notice shall be included in all
:: copies or substantial portions of the Software.

:: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
:: IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
:: FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
:: AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
:: LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
:: OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
:: SOFTWARE.
*/

#include <Common.hpp>

/**
 * @brief Class implementing system event log functionality
 */
class EventLog
{
    using HANDLE_t = HANDLEImpl_t<decltype(&DeregisterEventSource)>;

    HANDLE_t Source{nullptr, &DeregisterEventSource};

public:
    EventLog(void) = default;
    ~EventLog(void) = default;
    EventLog(EventLog const&) = delete;
    EventLog(EventLog&&) = delete;
    EventLog& operator=(EventLog const&) = delete;

    bool Register(_In_z_ PCWSTR LogName);
    bool Log(_In_z_ PCWSTR Message);
};

bool EventLog::Register(_In_z_ PCWSTR LogName)
{
    Source.reset(RegisterEventSourceW(nullptr,  LogName));
    if (!Source)
    {
        OutputDebugStringW(L"Can't register event log\n");
        return false;
    }

    return true;
}

bool EventLog::Log(_In_z_ PCWSTR Message)
{
    PCWSTR strings[] = { Message };

    if (!ReportEventW(Source.get(),
                      EVENTLOG_INFORMATION_TYPE,
                      0,
                      0,
                      nullptr,
                      _countof(strings),
                      0,
                      strings,
                      nullptr))
    {
        OutputDebugStringW(L"Can't log to event log\n");
        return false;
    }

    return true;
}

