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

/**
 * @brief SCM cleanup
*/
SCM::~SCM(void)
{
    if (Service)
    {
        (void)StopService();
    }
}

/**
 * @brief Setup service and start it
 *
 * @return true if successful
 */
bool SCM::Initialize(void)
{
    _ASSERT(!Manager);
    _ASSERT(!Service);

    DWORD lastError;

    Manager.reset(OpenSCManagerW(nullptr,  nullptr, ManagerDesiredAccess_k));
    if (!Manager)
    {
        lastError = GetLastError();
        if (ERROR_ACCESS_DENIED == lastError)
        {
            std::cerr << "Restart the program with admin rights" << std::endl;
        }
        else
        {
            UserErrorMessage(L"OpenSCManager() failure", lastError);
        }

        return false;
    }

    // Q: Does the service already exists?
    Service.reset(OpenServiceW(Manager.get(), SVCNAME, ServiceDesiredAccess_k));
    if (!Service)
    {
        lastError = GetLastError();
        switch(lastError)
        {
            case ERROR_SERVICE_DOES_NOT_EXIST:
                // Need to create the service
                return RegisterService();

            default:
                UserErrorMessage(L"OpenServiceW() failure", lastError);
                return false;
        }
    }

    // Service existing and ready to go
    return StartService();
}

/**
 * @brief Register service DLL using svchost.exe
 *
 * @return true if successful
 */
bool SCM::RegisterService(void)
{
    _ASSERT(Manager);
    _ASSERT(!Service);

    std::cout << "Registering new umock service..." << std::endl;
    // Create the service for the first time
    if (!CreateServiceW(Manager.get(),
                        SVCNAME,
                        L"umock programming challenged service",
                        ServiceDesiredAccess_k,
                        SERVICE_WIN32_OWN_PROCESS,
                        SERVICE_DEMAND_START,
                        SERVICE_ERROR_NORMAL,
                        L"C:\\Windows\\System32\\svchost.exe -k " SVCNAME,
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
    if (!GetModuleFileNameW(nullptr, serviceDllPath, _countof(serviceDllPath)))
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
    if (!svcParametersKey.AddStringValue(L"ServiceMain", L"ServiceMain", REG_SZ))
    {
        return false;
    }

    Service.reset(OpenServiceW(Manager.get(), SVCNAME, ServiceDesiredAccess_k));
    if (!Service)
    {
        return false;
    }

    return StartService();
}

/**
 * @brief Start service DLL using svchost.exe
 *
 * @return true if successful
 */
bool SCM::StartService(void)
{
    _ASSERT(Service);

    DWORD lastError;

    std::cout << "Starting umock service..." << std::endl;

    // Try to start the service
    if (!::StartServiceW(Service.get(), 0, nullptr))
    {
        lastError = GetLastError();
        if (ERROR_SERVICE_ALREADY_RUNNING == lastError)
        {
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
                std::cout << "Service " << SVCNAME << " started successfully" << std::endl;
                return  true;

            case SERVICE_STOP_PENDING:
                std::cout << "Waiting for " << SVCNAME << "service to start..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(5));
                break;

            default:
                std::cerr << "Unable to start service: (" << SVCNAME
                          << "). -  Current state is: " << ServiceStatus.dwCurrentState<< std::endl;
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
bool SCM::StopService(void)
{
    _ASSERT(Service);

    SERVICE_STATUS ServiceStatus{};

    std::cout << "Stopping umock service..." << std::endl;

    // Try to stop the service
    if (!::ControlService(Service.get(), SERVICE_CONTROL_STOP, &ServiceStatus))
    {
        UserErrorMessage(L"ControlService() failure");
        return false;
    }

    // Wait for the service to fully stop for about a minute
    for (int attempts{}; attempts < 12; attempts++)
    {
        switch (ServiceStatus.dwCurrentState)
        {
            case SERVICE_STOPPED:
                std::cout << "Service " << SVCNAME << " shutdown successfully" << std::endl;
                return  true;

            case SERVICE_STOP_PENDING:
                std::cout << "Waiting for " << SVCNAME << "service to stop..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(5));

                if (!QueryServiceStatus(Service.get(), &ServiceStatus))
                {
                    UserErrorMessage(L"QueryServiceStatus() failure");
                    return false;
                }
                break;

            default:
                std::cerr << "Unable to stop service: (" << SVCNAME
                          << "). -  Current state is: " << ServiceStatus.dwCurrentState<< std::endl;
                return false;
        }
    }

    return false;
}

/**
 * @brief Delete service DLL using svchost.exe
 *
 * @todo Remove the entries from svchost.exe key (SvchostKey_k)
 *
 * @return true if successful
 */
bool SCM::DeleteService(void)
{
    _ASSERT(Service);

    std::cout << "Deleting umock service..." << std::endl;

    // Create the service for the first time
    if (!::DeleteService(Service.get()))
    {
        UserErrorMessage(L"DeleteService() failure");
        return false;
    }

    return true;
}
