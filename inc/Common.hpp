/**
  @brief Common definitions
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

#pragma once

#include <windef.h>
#include <winbase.h>
#include <handleapi.h>
#include <memory>
#include <sstream>
#include <iostream>

template<typename HandleCloser_t = decltype(&CloseHandle)>
using HANDLEImpl_t = std::unique_ptr<std::remove_pointer<HANDLE>::type, HandleCloser_t>;
using HANDLE_t = HANDLEImpl_t<>;

#define SVCNAME L"UmockSvc"
#define SVCMODULE L"Service.dll"

/**
 * @brief Message output for service
 *
 * @param[in] Message - Error message to print
 *
 * @TODO: Send to system event log instead of to debugger
 */
inline void ServiceMessage(_In_ const wchar_t* Message)
{
    OutputDebugStringW(Message);
    OutputDebugStringW(L"\n");
}

/**
 * @brief Formate error message for service process
 *
 * @param[in] Message - Error message to print
 * @param[in] Error - Optional system error code (default to last error)
 *
 * @TODO: Send to system event log instead of to debugger
 */
inline void ServiceErrorMessage(_In_ const wchar_t* Message,
                                _In_ DWORD Error = GetLastError())
{
    try
    {
        std::wostringstream message;
        std::error_code ec(Error, std::system_category());
        // Poor man's unicode translator
        const std::string error = std::move(ec.message());
        const std::wstring werror(error.cbegin(), error.cend());

        message << Message << L": (" << ec.value()
                << L") - " << werror << L"!" << std::endl;
        OutputDebugStringW(message.str().c_str());
    }
    catch(std::exception const&)
    {
        ServiceMessage(Message);
    }
}

/**
 * @brief Message output for user process
 *
 * @param[in] Message - Error message to print
 *
 * @TODO: Send to system event log instead of to debugger
 */
inline void UserMessage(_In_ const wchar_t* Message)
{
    std::wcout << Message << std::endl;
}

/**
 * @brief Formate error message for user process
 *
 * @param[in] Message - Error message to print
 * @param[in] Error - Optional system error code (default to last error)
 *
 * @TODO: Send to system event log instead of to debugger
 */
inline void UserErrorMessage(_In_ const wchar_t* Message,
                             _In_ DWORD Error = GetLastError())
{
    try
    {
        std::wostringstream message;
        std::error_code ec(Error, std::system_category());
        // Poor man's unicode translator
        const std::string error = std::move(ec.message());
        const std::wstring werror(error.cbegin(), error.cend());

        std::wcerr << Message << L": (" << ec.value()
                   << L") - " << werror << L"!" << std::endl;
    }
    catch(std::exception const&)
    {
        UserMessage(Message);
    }
}
