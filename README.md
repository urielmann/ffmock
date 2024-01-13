[![Build and Test x64-Debug](https://github.com/urielmann/ffmock/actions/workflows/cmake-x64-debug.yml/badge.svg)]  
[![Build and Test x86-Debug](https://github.com/urielmann/ffmock/actions/workflows/cmake-x64-debug.yml/badge.svg)]  

# **ffmock** - Microsoft Win32 API mocking library
## Table of Content

- [**ffmock** - Microsoft Win32 API mocking library](#ffmock---microsoft-win32-api-mocking-library)
  - [Table of Content](#table-of-content)
  - [General](#general)
  - [How to Add a Free Function Mock](#how-to-add-a-free-function-mock)
    - [Implementing Free Function Mock](#implementing-free-function-mock)
    - [Using the Mocks in Your Unit Tests](#using-the-mocks-in-your-unit-tests)
    - [Linking Free Function Mocks into Unit Tests](#linking-free-function-mocks-into-unit-tests)
    - [Define mocks for all APIs](#define-mocks-for-all-apis)
    - [Mangle mocked APIs' names](#mangle-mocked-apis-names)
    - [Host all mocks in a separate DLL.](#host-all-mocks-in-a-separate-dll)
  - [Troubleshooting](#troubleshooting)
## General
This library is a header only package to provide simple and versatile method of mocking Microsoft's Win32 native API. Most mocking packages are geared to mocking C++ objects (classes, structs). It makes it easy to inject dependencies, if the mocks are wrapped in an interface. They are not designed to mock free functions. For example, the popular Google Test (gtest) tells you that you need to [wrap free functions into interfaces](https://github.com/google/googletest/blob/main/docs/gmock_cook_book.md#mocking-free-functions). While this approach would work, it presents both an additional work, as well as, additional executional overhead.

The **ffmock** package is a light weight method to directly replace free functions with a mock. It has the advantage that any trace of the mock completely disappears once the final production code compiles. The free functions are linked directly into your executable from the Microsoft's libraries. Additionally, there's no need write any additional code to use the free functions in order to mock them. As such, the same mocks can be used to in multiple projects without changes. Additionally, if you are wrapping the same functions into different classes you may need to modify your code as more methods are added to for other functions - which would be the case if the code was enhanced.  

Another excellent use case for **ffmock** is unit testing driver kernel code or Windows services. Driver and some of the services API cannot be called in regular user-mode processes. Mocking these functions allow the logic to be tested for both success and fail cases.  

## How to Add a Free Function Mock
In order to mock any free function in your unit tests you need to allow the linker to link in a replacement function with the exact same name and signature as the original API. The mock allows full control over the execution of the original API. At this point the function can provide the original functionality by calling into Microsoft's free function implementing the API, or return any other outcome as needed. Since the Win32 API are all hosted in system loadable modules (system DLLs provided by Microsoft), these can be easily substituted.  
The functionality provided by the system is linked in using import libraries. These provide your code with import table entries to call into the system DLLs. **Ffmock** substitute these entries with an actual function with the exact same parameters. When any of these functions are called, your unit tests are in full control of the action taken. It allows to check the call's parameters values, return specific values as the result of the call, or populate any out-params with modified values to be sent to your code.

### Implementing Free Function Mock
Each mock is made of two parts:  
  1. Templated class with general parameters
  2. The substituting of the original API with a method of the same name and signature. The function has static instance of the above class to control execution.

Here's an example of the required code for mocking [**RegisterServiceCtrlHandlerW**](demo/tst/Mocks.hpp#L44) and [**SetServiceStatus**](demo/tst/Mocks.hpp#L66). If you are writing a service process, you'll need to call these functions. However, calling them outside a service process is invalid, so you can't test your code in a regular unit test.  
```C++
// Mocks.hpp
#include <ffmock/ffmock.h>
#include <winsvc.h>


namespace Mocks
{

/*****************************************************************
 * @brief Mocked APIs for services
 *****************************************************************/

/**
 * @brief Mock for RegisterServiceCtrlHandlerW
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-registerservicectrlhandlerw
 */
class FFRegisterServiceCtrlHandlerW
    : public ::ffmock::Mock<SERVICE_STATUS_HANDLE, decltype(::RegisterServiceCtrlHandlerW), nullptr, ERROR_NOT_ENOUGH_MEMORY>
{
    friend
    FFMOCK_IMPORT
    SERVICE_STATUS_HANDLE
    WINAPI
    ::RegisterServiceCtrlHandlerW(
        _In_ LPCWSTR            ServiceName,
        _In_ __callback
             LPHANDLER_FUNCTION HandlerProc
        );

    FFRegisterServiceCtrlHandlerW(HMODULE Module) : Mock_t(Module, "RegisterServiceCtrlHandlerW")
    {
    }
};

/**
 * @brief Mock for SetServiceStatus
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-setservicestatus
 */
class FFSetServiceStatus
    : public ::ffmock::Mock<BOOL, decltype(::SetServiceStatus), FALSE, ERROR_INVALID_HANDLE>
{
    friend
    FFMOCK_IMPORT
    BOOL
    WINAPI
    ::SetServiceStatus(
        _In_ SERVICE_STATUS_HANDLE ServiceHandle,
        _In_ LPSERVICE_STATUS      ServiceStatus
        );

    FFSetServiceStatus(HMODULE Module) : Mock_t(Module, "SetServiceStatus")
    {
    }
};

} // namespace Mocks
```
Note that the __*friend*__ functions' declarations were taken verbatim out of the Microsoft headers.  

For convenience, a [preprocessor macro](inc/ffmock/ffmock.h#L206) is defined and can be used for each of the class declarations.
``` C++
/**
 * @brief Declaration of mocked Win32 API
 *
 * @param API_NAME - The API being mocked
 * @param RET_TYPE - Return type of the API
 * @param RET_ERROR - Default value to return when the API fails
 * @param LAST_ERROR - Win32 API commonly set last error code to be retrieved by
 *                     GetLastError(). This is always used for functions returning
 *                     BOOL and the return value is set to FALSE.
 * @param CALL_TYPE - Function calling convention
 * @param CALL_ARGS - Parenthesize list of API arguments
 */
#define DECLARE_MOCK(API_NAME, RET_TYPE, RET_ERROR, LAST_ERROR, CALL_TYPE, CALL_ARGS)
```

So the previous code can also be written as:

```C++
// Mocks.hpp
#include <ffmock/ffmock.h>
#include <winsvc.h>


namespace Mocks
{

/*****************************************************************
 * @brief Mocked APIs for services
 *****************************************************************/

/**
 * @brief Mock for RegisterServiceCtrlHandlerW
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-registerservicectrlhandlerw
 */
DECLARE_MOCK(RegisterServiceCtrlHandlerW, SERVICE_STATUS_HANDLE, nullptr, ERROR_NOT_ENOUGH_MEMORY, WINAPI,
    (
        _In_ LPCWSTR            ServiceName,
        _In_ __callback
             LPHANDLER_FUNCTION HandlerProc
    ));

/**
 * @brief Mock for SetServiceStatus
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-setservicestatus
 */
DECLARE_MOCK(SetServiceStatus, BOOL, FALSE, ERROR_INVALID_HANDLE, WINAPI,
    (
    _In_ SC_HANDLE Service
    ));

} // namespace Mocks
```
The actual mock needs to be defined in a C++ source file. Here's an example of the two mocks from the previous example. It starts with defining a global handle for Advapi32.dll. Next, a macro is used to create instances of mocks' static data members followed by the mocked free functions.
```C++
// Mocks.cpp
#include "Mocks.hpp"

#pragma warning(disable:4273) // inconsistent dll linkage

static HMODULE AdvAPI32{LoadLibraryW(L"advapi32.dll")};

/**
 * @brief Instances of the mock's Guard class members
 */
DEFINE_GUARD(Mocks, RegisterServiceCtrlHandlerW);
DEFINE_GUARD(Mocks, SetServiceStatus);

/**
 * @brief Instances of the mock's class static members
 */
DEFINE_MOCK(RegisterServiceCtrlHandlerW, SERVICE_STATUS_HANDLE, nullptr, ERROR_NOT_ENOUGH_MEMORY);
DEFINE_MOCK(SetServiceStatus, BOOL, FALSE, ERROR_INVALID_HANDLE);

extern "C"
{

/*****************************************************************
 * @brief Mocked APIs for services
 *****************************************************************/

_Must_inspect_result_
FFMOCK_IMPORT
SERVICE_STATUS_HANDLE
WINAPI
RegisterServiceCtrlHandlerW(
    _In_ LPCWSTR            ServiceName,
    _In_ __callback
         LPHANDLER_FUNCTION HandlerProc
    ) try
{
    static Mocks::FFRegisterServiceCtrlHandlerW mock(AdvAPI32);
    return mock(ServiceName, HandlerProc);
}
catch(std::bad_alloc const&)
{
    return nullptr;
}

FFMOCK_IMPORT
BOOL
WINAPI
SetServiceStatus(
    _In_ SERVICE_STATUS_HANDLE ServiceHandle,
    _In_ LPSERVICE_STATUS      ServiceStatus
    ) try
{
    static Mocks::FFSetServiceStatus mock(AdvAPI32);
    return mock(ServiceHandle, ServiceStatus);
}
catch(std::bad_alloc const&)
{
    return FALSE;
}

} // extern "C"
```

### Using the Mocks in Your Unit Tests
Once the mocks are defined, using them in a unit test is trivial. Use the mock's [**Guard**](inc/ffmock/ffmock.h#L167) nested class to assure that the API call will fail, or to modify the API's behavior. The **Guard** will substitute the call to the real implementation. If no argument is given to the **Guard** instance, any call to the mocked API will return the value specified in the [**RetType Error**](inc/ffmock/ffmock.h#L87) of the Mock template class. If desired, the value returned by SetLastError() can also be controlled by providing the requested value as the [**DWORD Error2Set**](inc/ffmock/ffmock.h#L87) template parameter.  
Occasionally, there's a need to have a more elaborate modification to the API behavior. This can be, returning specific value to an out-param of the API, checking any of the argument values passed to the API, or failing the API after the Nth call, etc. Such action can be achieved by providing a lambda instance with the desired logic. Such lambda must have the exact same signature as the mocked API, including the parameters types and the return value type.  
Here's an example:
```C++
/******************************************************
 * @brief Service class unit tests
 *
 * @todo Full functional and branch coverage
 ******************************************************/
class ServiceTestSuite : public testing::Test
                       , public Service
{
protected:
};

TEST_F(ServiceTestSuite, Test_ServiceMain_Failed)
{
    wchar_t* argv[] = {L"UnitTest"};
    Mocks::FFRegisterServiceCtrlHandlerW::Guard rschGuard;
    Mocks::FFSetServiceStatus::Guard sssGuard(
        [](_In_ SERVICE_STATUS_HANDLE ServiceHandle,
           _In_ LPSERVICE_STATUS      ServiceStatus) -> BOOL
        {
            EXPECT_EQ(ServiceHandle, nullptr);
            EXPECT_TRUE(ServiceStatus->dwWin32ExitCode == ERROR_NOT_ENOUGH_MEMORY);
            EXPECT_TRUE(ServiceStatus->dwCurrentState == SERVICE_STOPPED);
            return TRUE;
        });

    // The call to Service::ServiceMain() will invoke both mocked APIs
    Service::ServiceMain(_countof(argv), argv);

    ASSERT_FALSE(SvcStatusHandle);
}
```

### Linking Free Function Mocks into Unit Tests
Microsoft added a special feature to its linker to prevent it from accidentally linking a function with the same signature or name as any of the Win32 API. If you link a function with the same name as an exiting Win32 API function, the linker will issue an error. For example:  
> *advapi32.lib(ADVAPI32.dll) : error LNK2005: **ControlService** already defined in Mocks.obj  
>   Creating library bin\x64\FFmockUnitTests.lib and object bin\x64\FFmockUnitTests.exp  
> bin\x64\FFmockUnitTests.exe : fatal error LNK1169: one or more multiply defined symbols found*   

There are three ways to deal with this issue. Each has its advantages and disadvantages. To select the right linkage for your unit tests it is important to first understand the implications.  

### Define mocks for all APIs
If all Win32 API functions hosted in a specific DLL are mocked, you can remove the dependency on the Microsoft library and the linker will resolve all of Microsoft's library symbols from the mocks. This has the disadvantage of having to add a mock function even if none of the unit tests uses it.  

### Mangle mocked APIs' names
This will require each mocked function to be given an alternative name using compile's definitions on the command line (e.g., **/DControlService=__mock_ControlService)**. While this is a relatively simple workaround, it has some significant disadvantages.
 * If many APIs are mocked, the compiler command line becomes cluttered with definitions. The resulting command line could run out of the executing shell limits
 * The tested code will have to be directly included in the unit test, or a library with the mangled names will be needed in addition to library of production code.

The above choices, are required when the mocks are statically linked to the unit tests. The next section describes the third choice, when mocks are hosted in their own DLL. In such case, the names are never mangled, and you are free to define as few, or as many, mocks as required.  

### Host all mocks in a separate DLL.
While creating a DLL with mocks is an additional step, the final result has many advantages:  
 * If could help reduce the size the entire build takes on disk. If the same mock library is shared by multiple unit tests, all unit tests will use a single instance of the same DLL.  
 The mocks are implementing the mocked action using [*std::function*](https://en.cppreference.com/w/cpp/utility/functional/function) objects. These classes require the standard library C++ runtime support. On Windows, the support is implemented for each module separately. While the functionality is similar for all modules, the modules each carry their own implementation. The per-module implementation is needed because when a module is loaded or unloaded, global and static scope instances are allocated or released.  
 In the cases of statically linked mocks above, the runtime support is in the executable itself. When the mocks are in a DLL the mocks must be linked to assure the correct runtime support is used. This is the reason that the Guard class implementation, mock class static members, and the mock function itself, all have to be in the same binary as the mock implementation.  
 Example:  
 ```C++
 // Mocks.cpp
 #include "Mocks.hpp"

// Define Guard methods, and mock class static members
DEFINE_GUARD(Mocks, RegOpenKeyW);
DEFINE_MOCK(RegOpenKeyW, LSTATUS, RROR_REGISTRY_IO_FAILED, NO_ERROR);

// Mock API definition
FFMOCK_IMPORT
LSTATUS
APIENTRY
RegOpenKeyW(
    _In_     HKEY    Key,
    _In_opt_ LPCWSTR SubKey,
    _Out_    PHKEY   Result
    ) try
{
    static Mocks::FFRegOpenKeyW mock(AdvAPI32);
    return mock(Key, SubKey, Result);
}
catch(std::bad_alloc const&)
{
    return ERROR_OUTOFMEMORY;
}
 ```

 ## Troubleshooting
 * Duplicate symbols. Fixing this issue is described above
 > *advapi32.lib(ADVAPI32.dll) : error LNK2005: **ControlService** already defined in Mocks.obj  
>   Creating library bin\x64\FFmockUnitTests.lib and object bin\x64\FFmockUnitTests.exp  
> bin\x64\FFmockUnitTests.exe : fatal error LNK1169: one or more multiply defined symbols found*   
 * Duplicate template instantiation. Code such as this:  
```C++
DEFINE_MOCK(RegCreateKeyW, LSTATUS, ERROR_REGISTRY_CORRUPT, NO_ERROR);
DEFINE_MOCK(RegOpenKeyW, LSTATUS, ERROR_REGISTRY_CORRUPT, NO_ERROR);
```
Can cause the following errors:  
 > *C:\Play\ffmock\tst\Mocks.cpp(45,1): error C2084: function 'ffmock::Mock<LSTATUS,LSTATUS (HKEY,LPCWSTR,PHKEY),1015,0>::Guard::Guard(std::function<long (HKEY,LPCWSTR,PHKEY)> &&)' already has a body*  
 > *C:\Play\ffmock\tst\Mocks.cpp(45,1): error C2084: function 'ffmock::Mock<LSTATUS,LSTATUS (HKEY,LPCWSTR,PHKEY),1015,0>::Guard::~Guard(void)' already has a body*  
 > *C:\Play\ffmock\tst\Mocks.cpp(45,1): error C2995: 'void ffmock::Mock<LSTATUS,LSTATUS (HKEY,LPCWSTR,PHKEY),1015,0>::Guard::Set(const std::function<long (HKEY,LPCWSTR,PHKEY)> &)': function template has already been defined*  
 > *C:\Play\ffmock\tst\Mocks.cpp(45,1): error C2995: 'void ffmock::Mock<LSTATUS,LSTATUS (HKEY,LPCWSTR,PHKEY),1015,0>::Guard::Clear(void)': function template has already been defined*   
 
 This error is a result of the base template class for one or more mock classes has the same template parameters. In this case both, *RegCreateKeyW()*, and *RegOpenKeyW()* functions having the same signature (i.e., *long (HKEY,LPCWSTR,PHKEY)*). To fix this issue make sure the instances are unique by providing different return error code, or setting different values for *SetLastError()*.  
 Example:  
 ```C++
DEFINE_MOCK(RegCreateKeyW, LSTATUS, ERROR_REGISTRY_CORRUPT, NO_ERROR);
DEFINE_MOCK(RegOpenKeyW, LSTATUS, ERROR_REGISTRY_IO_FAILED, NO_ERROR);
```
 * The mocked API is not getting linked. The linker uses the Microsoft library API.  
 This is a result of the mock APIs DLL listed an incorrect place on the linker's command line. Changing the order of libraries for the linker will probably solve this issue.  
 Make also sure that the original export link library is not forced into you linker by a [comment pragma](https://learn.microsoft.com/en-us/cpp/preprocessor/comment-c-cpp?view=msvc-170) (e.g., *#pragma comment(lib, "emapi")*) in your code.  
