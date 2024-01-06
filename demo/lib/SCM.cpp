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

#include "SCM.hpp"
#include "Registry.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

/**
 * @brief SCM cleanup
*/
SCM::~SCM(void)
{
    if (Service)
    {
        (void)Stop();
    }
}

/**
 * @brief Open SCM and the service
 *
 * @details If the service does not exist, the operation is successful, and the
 *          returned LastError is set to ERROR_SERVICE_DOES_NOT_EXIST.
 * @param LastError - Return the last error
 *
 * @return true if successful
 */
bool SCM::Open(_Out_ DWORD LastError)
{
    _ASSERT(!Manager);
    _ASSERT(!Service);

    Manager.reset(OpenSCManagerW(nullptr, nullptr, ManagerDesiredAccess_k));
    if (!Manager)
    {
        LastError = GetLastError();
        if (ERROR_ACCESS_DENIED == LastError)
        {
            std::cerr << "Restart the program with admin rights" << std::endl;
        }
        else
        {
            UserErrorMessage(L"OpenSCManager() failure", LastError);
        }

        return false;
    }

    // Q: Does the service still exists?
    Service.reset(OpenServiceW(Manager.get(), SVCNAME, ServiceDesiredAccess_k));
    if (!Service)
    {
        LastError = GetLastError();
        switch (LastError)
        {
        case ERROR_SERVICE_DOES_NOT_EXIST:
            // No such service
            return true;

        default:
            UserErrorMessage(L"OpenServiceW() failure", LastError);
            return false;
        }
    }

    return true;
}

/**
 * @brief Setup service and start it
 *
 * @param[in] AdditionalArgs - Additional command line arguments
 *
 * @return true if successful
 */
bool SCM::Initialize(_In_opt_z_ char* AdditionalArgs)
{
    _ASSERT(!Manager);
    _ASSERT(!Service);

    DWORD lastError{};

    if (!Open(lastError))
    {
        return false;
    }


    // Service existing and ready to go
    return ERROR_SERVICE_DOES_NOT_EXIST == lastError ? Register(AdditionalArgs)
                                                     : Start(AdditionalArgs);
}

/**
 * @brief Register service DLL using svchost.exe
 *
 * @param[in] AdditionalArgs - Additional command line arguments for the registration.
 *
 * @see https://nasbench.medium.com/demystifying-the-svchost-exe-process-and-its-command-line-options-508e9114e747
 *
 * @return true if successful
 */
bool SCM::Register(_In_opt_z_ char* AdditionalArgs)
{
    _ASSERT(Manager);
    _ASSERT(!Service);

    std::wstring cmdLine;
    try
    {
        cmdLine = L"%SystemRoot%\\System32\\svchost.exe -k " SVCNAME;
        if (AdditionalArgs)
        {
            // Append any additional command line to be passed to the service
            // when it is starting
            cmdLine += L" ";
            const size_t length = strnlen(AdditionalArgs, MAX_PATH);
            cmdLine.append(AdditionalArgs, AdditionalArgs + length);
        }
    }
    catch(std::bad_alloc const&)
    {
        UserErrorMessage(__FUNCTIONW__ L" failure", ERROR_OUTOFMEMORY);
    }
    
    std::cout << "Registering new ffmock service..." << std::endl;
    // Create the service for the first time
    if (!CreateServiceW(Manager.get(),
                        SVCNAME,
                        L"ffmock programming sample service",
                        ServiceDesiredAccess_k,
                        SERVICE_WIN32_OWN_PROCESS,
                        SERVICE_DEMAND_START,
                        SERVICE_ERROR_NORMAL,
                        cmdLine.c_str(),
                        nullptr,
                        nullptr,
                        nullptr,
                        nullptr,
                        nullptr))
    {
        UserErrorMessage(L"CreateServiceW() failure");
        return false;
    }

    wchar_t serviceDllPath[MAX_PATH]{};
    // Assume the service DLL is in the same directory as this process image
    if (!GetModuleFileNameW(GetModuleHandleW(SVCMODULE), serviceDllPath, _countof(serviceDllPath)))
    {
        UserErrorMessage(L"GetModuleFileNameW() failure");
        return false;
    }

    wchar_t* slash = wcsrchr(serviceDllPath, L'\\');
    *slash = L'\0';
    wcscat_s(serviceDllPath, L"\\" SVCMODULE);

    // Set value for svchost.exe to point to the service
    Registry svchostKey;
    if (!svchostKey.Open(SvchostKey_k)
     || !svchostKey.AddStringValue(SVCNAME, SVCNAME L"\0", REG_MULTI_SZ))
    {
        return false;
    }

    // Create service parameters to provide svchost.exe with the module's information
    Registry svcParametersKey;
    if (!svcParametersKey.Create(SvcParametersKey_k))
    {
        return false;
    }

    // Set value for the service to point to the service DLL
    if (!svcParametersKey.AddStringValue(L"ServiceDLL", serviceDllPath, REG_EXPAND_SZ))
    {
        return false;
    }

    // Expose the DLL entrypoint for svchost.exe
    if (!svcParametersKey.AddStringValue(L"ServiceMain", L"Service::Main", REG_SZ))
    {
        return false;
    }

    Service.reset(OpenServiceW(Manager.get(), SVCNAME, ServiceDesiredAccess_k));
    if (!Service)
    {
        return false;
    }

    return Start(AdditionalArgs);
}

/**
 * @brief Start service DLL using svchost.exe
 *
 * param[in] AdditionalArgs - Additional arguments to pass to the starting service
 *
 * @return true if successful
 */
bool SCM::Start(_In_opt_z_ char* AdditionalArgs)
{
    _ASSERT(Service);

    std::cout << "Starting ffmock service..." << std::endl;

    std::vector<const wchar_t*> args;
    std::vector<wchar_t> additionalArgs;
    try
    {
        if (AdditionalArgs)
        {
            wchar_t* context;
            const size_t length{ strnlen(AdditionalArgs, MAX_PATH) };

            // Convert to UNICODE
            additionalArgs.assign(AdditionalArgs, AdditionalArgs + length + 1);
            // Tokenized the string into a multi-string
            for (wchar_t* token{ wcstok_s(&additionalArgs.front(), L" ", &context) };
                    token;
                 token = wcstok_s(nullptr, L" ", &context))
            {
                args.push_back(token);
            }
        }
    }
    catch (std::bad_alloc const&)
    {
        UserErrorMessage(L"StartServiceW() failure", ERROR_OUTOFMEMORY);
        return false;
    }

    // Try to start the service
    if (!StartServiceW(Service.get(), static_cast<DWORD>(args.size()), &args.front()))
    {
        DWORD lastError{ GetLastError() };
        if (ERROR_SERVICE_ALREADY_RUNNING == lastError)
        {
            // Prevent the service from shutting down when SCM class is destroyed
            Service.reset();
            return true;
        }

        UserErrorMessage(L"StartServiceW() failure", lastError);
        return false;
    }

    // Wait for the service to fully start for about a minute
    for (int attempts{}; attempts < 12; attempts++)
    {
        SERVICE_STATUS ServiceStatus{};

        // Check the service state
        if (!QueryServiceStatus(Service.get(), &ServiceStatus))
        {
            UserErrorMessage(L"QueryServiceStatus() failure");
            return false;
        }

        switch (ServiceStatus.dwCurrentState)
        {
            case SERVICE_RUNNING:
                std::wcout << L"Service " << SVCNAME << L" started successfully" << std::endl;
                // Prevent the service from shutting down when SCM class is destroyed
                Service.reset();
                return  true;

            case SERVICE_STOP_PENDING:
                std::wcout << L"Waiting for " << SVCNAME << L"service to start..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(5));
                break;

            default:
                std::wcerr << L"Unable to start service: (" << SVCNAME
                           << L"). -  Current state is: " << ServiceStatus.dwCurrentState<< std::endl;
                return false;
        }
    }

    return false;
}

/**
 * @brief Stop service DLL using svchost.exe
 *
 * @return true if successful
 */
bool SCM::Stop(void)
{
    _ASSERT(Service);

    SERVICE_STATUS ServiceStatus{};

    std::cout << "Stopping ffmock service..." << std::endl;

    // Try to stop the service
    if (!ControlService(Service.get(), SERVICE_CONTROL_STOP, &ServiceStatus))
    {
        if (SERVICE_STOPPED == ServiceStatus.dwCurrentState)
        {
            return true;
        }

        UserErrorMessage(L"ControlService() failure");
        return false;
    }

    // Wait for the service to fully stop for about a minute
    for (int attempts{}; attempts < 12; attempts++)
    {
        switch (ServiceStatus.dwCurrentState)
        {
            case SERVICE_STOPPED:
                std::wcout << L"Service " << SVCNAME << L" shutdown successfully" << std::endl;
                return  true;

            case SERVICE_STOP_PENDING:
                std::wcout << L"Waiting for " << SVCNAME << L"service to stop..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(5));

                if (!QueryServiceStatus(Service.get(), &ServiceStatus))
                {
                    UserErrorMessage(L"QueryServiceStatus() failure");
                    return false;
                }
                break;

            default:
                std::wcerr << L"Unable to stop service: (" << SVCNAME
                           << L"). -  Current state is: " << ServiceStatus.dwCurrentState<< std::endl;
                return false;
        }
    }

    return false;
}

/**
 * @brief Delete service DLL using svchost.exe
 *
 * @details Also remove the entries from svchost.exe key (SvchostKey_k)
 *
 * @return true if successful
 */
bool SCM::Delete(void)
{
    DWORD lastError{};

    if (!Open(lastError))
    {
        return false;
    }


    // Service existing and ready to go
    if (ERROR_SERVICE_DOES_NOT_EXIST == lastError)
    {
        return true;
    }

    // First stop the service
    if (!Stop())
    {
        return false;
    }

    std::cout << "Deleting ffmock service..." << std::endl;

    // Create the service for the first time
    if (!::DeleteService(Service.get()))
    {
        UserErrorMessage(L"DeleteService() failure");
        return false;
    }

    std::cout << "Deleting svchost.exe entries..." << std::endl;

    // Clear value for svchost.exe pointing to the service
    Registry svchostKey;
    if (!svchostKey.Open(SvchostKey_k)
     || !svchostKey.DeleteStringValue(SVCNAME))
    {
        return false;
    }

    return true;
}
