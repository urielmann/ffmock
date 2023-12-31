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

#include "Service.hpp"
#include "SCM.hpp"
#include <winerror.h>
#include <synchapi.h>
#include <consoleapi.h>
#include <functional>
#include <map>

SERVICE_STATUS Service::SvcStatus =
{
    SERVICE_WIN32_OWN_PROCESS, // DWORD   dwServiceType
};

SERVICE_STATUS_HANDLE Service::SvcStatusHandle = nullptr;

size_t Service::SuccessfulSends;
size_t Service::FailedSends;
HANDLE_t Service::Event{nullptr, &CloseHandle};

/**
 * @brief Sets the current service status and reports it to the SCM.
 *
 * @param[in] CurrentState - Current service state
 * @param[in] Win32ExitCode - Win32 exit code to send to SCM
 * @param[in] WaitHint - Wait hint
 *
 * @return None
 */
void Service::ReportSvcStatus(_In_ DWORD CurrentState,
                              _In_ DWORD Win32ExitCode/* = NO_ERROR*/,
                              _In_ DWORD WaitHint/*= 0*/)
{
    // Fill in the SERVICE_STATUS structure.
    SvcStatus.dwCurrentState = CurrentState;
    SvcStatus.dwWin32ExitCode = Win32ExitCode;
    SvcStatus.dwWaitHint = WaitHint;

    SvcStatus.dwControlsAccepted = (CurrentState == SERVICE_START_PENDING)
                                 ? 0 : SERVICE_ACCEPT_STOP;

    if ((CurrentState == SERVICE_RUNNING) || (CurrentState == SERVICE_STOPPED))
        SvcStatus.dwCheckPoint = 0;
    else
        SvcStatus.dwCheckPoint++;

    // Report the status of the service to the SCM.
    if (!SetServiceStatus(SvcStatusHandle, &SvcStatus))
    {
        ServiceErrorMessage(L"SetServiceStatus");
    }
}

/**
 * @brief Called by SCM whenever a control code is sent to the service
 *        using the ControlService() function.
 *
 * @param[in] SvcCtrlCode - SCM control code
 *
 * @return None
 */
void WINAPI Service::SvcCtrlHandler(_In_ DWORD SvcCtrlCode)
{
   // Handle the requested control code.
   switch(SvcCtrlCode)
   {
      case SERVICE_CONTROL_STOP:
         ReportSvcStatus(SERVICE_STOP_PENDING);

         // Signal the service to stop.
        if (!SetEvent(Event.get()))
        {
            ServiceErrorMessage(L"SetEvent() failed");
        }

         ReportSvcStatus(SvcStatus.dwCurrentState);
         break;

      default:
         break;
   }
}

/**
 * @brief Use rundll32.exe to register the service
 * 
 * @param Wnd - Desktop Window handle
 * @param Instance - Application instance
 * @param CmdLine - Rundll32 command line
 * @param CmdShow - Window state
 */
__declspec(dllexport)
void CALLBACK Service::Register(_In_ HWND Wnd,
                                _In_ HINSTANCE Instance,
                                _In_  LPSTR CmdLine,
                                _In_  int CmdShow)
{
    using Action_t = std::function<bool(PSTR)>;
    using Map_t = std::map<std::string, Action_t>;

    UNREFERENCED_PARAMETER(Wnd);
    UNREFERENCED_PARAMETER(Instance);
    UNREFERENCED_PARAMETER(CmdShow);

    FILE* fp;
    // rundll32.exe is a Windows GUI app. Make sure the output goes to the
    // parent process console to have any error output visible.
    AttachConsole(ATTACH_PARENT_PROCESS);
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);

    SCM scm;

    if (CmdLine && !*CmdLine)
    {
        // No arguments given to rundll32.exe
        if (!scm.Initialize(CmdLine))
        {
            return;
        }
        return;
    }

    char* space = strchr(CmdLine, ' ');
    if (space)
    {
        *space = '\0';
        // Move to next token
        while (*++space && ' ' != *space)
            ;
        // Q: Are there additional arguments given
        if (!*space) space = nullptr;
    }

    try
    {
        const size_t length = strnlen_s(CmdLine, MAX_PATH);
        _strlwr_s(CmdLine, length + 1);
        Map_t actions{
            { "register", [&scm](_In_ char* Args)
                {
                    return scm.Initialize(Args);
                }
            },
            { "delete", [&scm](_In_ char* Args)
                {
                    UNREFERENCED_PARAMETER(Args);
                    return scm.DeleteService();
                }
            },
        };

        auto action{actions.find(CmdLine)};
        if (actions.cend() == action)
        {
            if (!scm.Initialize(space))
            {
                return;
            }
        }
        else
        {
            if (!action->second(space))
            {
                return;
            }
        }
    }
    catch(std::bad_alloc const &)
    {
        UserErrorMessage(__FUNCTIONW__ L" failure", ERROR_OUTOFMEMORY);
        return;
    }
}

/**
 * @brief Entry point for the service
 *
 * @param[in] Argc - Number of arguments in the Argv array
 * @param[in] Argv - Array of strings. The first string is the name of
 *                   the service and subsequent strings are passed by the process
 *                   that called the StartService function to start the service.
 *
 * @note This method is exported for use by svchost.exe when the DLL is loaded. See
 *       dll.def for more info.
 * @return None
 */
__declspec(dllexport)
void WINAPI Service::ServiceMain(_In_ DWORD Argc, _In_ PWSTR* Argv)
{
    _ASSERT(Argc);
    UNREFERENCED_PARAMETER(Argc);

    DWORD lastError;

    // Register the handler function for the service
    SvcStatusHandle = RegisterServiceCtrlHandlerW(Argv[0], &SvcCtrlHandler);
    if (!SvcStatusHandle)
    {
        lastError = GetLastError();
        ServiceErrorMessage(L"Failed to register service", lastError);
        ReportSvcStatus(SERVICE_STOPPED, lastError, 0);
        return;
    }

    // Report initial status to the SCM
    ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

    lastError = Initialize();
    if (!lastError)
    {
        // Report running status when initialization is complete.
        ReportSvcStatus(SERVICE_RUNNING);
        lastError = Wait();
    }

    ReportSvcStatus(SERVICE_STOPPED, lastError);
}

/**
 * @brief Initialize the service
 * 
 * @return NO_ERROR if successful 
 */
DWORD Service::Initialize(void)
{
    _ASSERT(!Event);

    Event.reset(CreateEventW(nullptr, false, false, nullptr));
    if (!Event)
    {
        DWORD lastError{GetLastError()};
        ServiceErrorMessage(L"CreateEventW() failed", lastError);
        return lastError;
    }

    return NO_ERROR;
}

/**
 * @brief Wait for event to trigger
 * 
 * @return NO_ERROR if successful 
 */
DWORD Service::Wait(void)
{
    for (;;)
    {
        switch (DWORD status{WaitForSingleObject(Event.get(), PollPeriod_k)})
        {
            case WAIT_OBJECT_0:
                return NO_ERROR;

            case WAIT_TIMEOUT:
                break;

            default:
                ServiceErrorMessage(L"WaitForSingleObject() failed", status);
                return status;
        }
    }
}
