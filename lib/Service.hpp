/**
  @brief DLL to be loaded onto svchost.exe for receiving list of running processes
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

#include <Common.hpp>
#include <winsvc.h>

/**
 * @brief Class providing basic functionality for SCM interaction
 */
class Service
{
    /**
     * @brief Allow access to privets by the unit tests
     */
protected:

    /**
     * @brief Wait period between list transmissions
     */
    static constexpr DWORD PollPeriod_k = 1000U * 60U;
    static constexpr DWORD WaitPeriod_k = 1000U * 2U;

    static SERVICE_STATUS SvcStatus;
    static SERVICE_STATUS_HANDLE SvcStatusHandle;
    static  HANDLE_t Event;

    static size_t SuccessfulSends;
    static size_t FailedSends;

    Service(void) = default;
    ~Service(void) = default;

    Service(Service const&) = delete;
    Service(Service&&) = delete;
    Service& operator=(Service const&) = delete;

    static DWORD Initialize(void);
    static DWORD Wait(void);
    static void ReportSvcStatus(_In_ DWORD CurrentState,
                                _In_ DWORD Win32ExitCode = NO_ERROR,
                                _In_ DWORD WaitHint = 0);
    static void WINAPI SvcCtrlHandler(_In_ DWORD SvcCtrlCode);
    __declspec(dllexport)
    static void CALLBACK Register(_In_ HWND Wnd,
                                  _In_ HINSTANCE Instance,
                                  _In_  LPSTR CmdLine,
                                  _In_  int CmdShow);
    __declspec(dllexport)
    static void WINAPI ServiceMain(_In_ DWORD /*Argc*/, _In_ PWSTR* /*Argv*/);
};
