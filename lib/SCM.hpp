/**
  @brief Service Control Manager utility class
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
 * @brief Class implementing service control manager (SCM) functionality
 */
class SCM
{
    using HANDLE_t = std::unique_ptr<std::remove_pointer<SC_HANDLE>::type, decltype(&CloseServiceHandle)>;

    static constexpr const wchar_t SvchostKey_k[] = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost";
    static constexpr const wchar_t SvcParametersKey_k[] = L"SYSTEM\\CurrentControlSet\\Services\\" SVCNAME L"\\Parameters";

    static constexpr DWORD ManagerDesiredAccess_k{ SC_MANAGER_CREATE_SERVICE
                                                 | SC_MANAGER_CONNECT
                                                 | STANDARD_RIGHTS_WRITE
                                                 | STANDARD_RIGHTS_READ };
    static constexpr DWORD ServiceDesiredAccess_k{ SERVICE_CHANGE_CONFIG
                                                 | SERVICE_QUERY_STATUS
                                                 | SERVICE_START
                                                 | SERVICE_STOP
                                                 | DELETE };

    /**
     * @brief Allow access to privets by the unit tests
     */
protected:

    HANDLE_t Manager{nullptr, &CloseServiceHandle};
    HANDLE_t Service{nullptr, &CloseServiceHandle};

public:
    SCM(void) = default;
    ~SCM(void);

    SCM(SCM const&) = delete;
    SCM(SCM&&) = delete;
    SCM& operator=(SCM const&) = delete;

    bool Initialize(_In_opt_z_ char* AdditionalArgs);
    bool RegisterService(_In_opt_z_ char* AdditionalArgs);
    bool StartService(void);
    bool StopService(void);
    bool DeleteService(void);
};
