# **umock** - Microsoft Win32 API mocking library
## General
This library is a header only package to provide simple and versatile method of mocking Microsoft's Win32 native API. Most mocking packages are geared to mocking C++ objects. It makes it easy to inject dependencies if wrapped in an interfaces. The are not designed to mock free functions. For example the popular Google Test (gtest) tells you that you need to [wrap free functions into interfaces](https://github.com/google/googletest/blob/main/docs/gmock_cook_book.md#mocking-free-functions). While this approach would work it present both an additional work as well as additional executional overhead.
The umock package is a light weight method to directly replace free functions with a mock. It has the advantage that any trace of the mock completely disappears once the final code compiles. The free functions are linked directly into your executable. Additionally, there's no need write any additional code to use the free functions in order to mock them. As such, the same mocks can be used to in multiple projects without changes. Wrapping the same functions into different classes you may need to modify your code as more methods are added to for other functions - which would be the case if the code was enhanced.  

## How to Add a Free Function Mock
In order to mock any free function in your unit tests you need to allow the linker to link in a replacement function with the exact same signature as the original API. The mock allow full control over the execution of the original API. At this point the function can provide the original functionality by calling into the free function implementing the API, or return any other outcome as needed. Since the Win32 API are all hosted in system loadable modules (system DLLs provided by Microsoft), these can be easily substituted.  
The functionality provided by the system is linked in using import libraries. These provide your code with import table entries to call into the system DLLs. Umock substitute these entries with an actual function with the exact same parameters. When any of these functions are called, your unit tests are in full control of the action taken. It allowed to check the call's parameters values. Return specific values as the result of the call, or populate any out-params with modified values to be sent to your code.


### Linking Free Function Mocks into Unit Tests
Microsoft added special feature to its linker to prevent from accidentally linking a function with the same signature or name as any of the Win32 API. If you link a function with the same name as an exiting Win32 API function, the linker will issue an error. For example:  
> *advapi32.lib(ADVAPI32.dll) : error LNK2005: **ControlService** already defined in Mocks.obj  
>   Creating library bin\x64\UmockUnitTests.lib and object bin\x64\UmockUnitTests.exp  
> bin\x64\UmockUnitTests.exe : fatal error LNK1169: one or more multiply defined symbols found*   

There are three ways to deal with this issue. Each has its advantages and disadvantages.  

#### Define mocks for all APIs
If all Win32 API functions hosted in a specific DLL are mocked, you can remove the dependency on the Microsoft library and the linker will resolve all of Microsoft's library symbols from the mocks. This has the disadvantage of having to add a mock function even if none of the unit tests uses it.  

#### Mangle mocked APIs' names
This will require each mocked function to be given an alternative name using compile's definitions on the command line (e.g., /DControlService=__mock_ControlService). While this is a relatively simple workaround it has some significant disadvantages.
 * If many APIs are mocked, the compiler command line becomes cluttered with definitions. The resulting command line could run out of the executing shell limits
 * The tested code will have to be directly included in the unit test, or a library with the mangled names will be needed in addition to library of production code.

#### Host all mocks in a separate DLL.
While creating a DLL with mocks is an additional step, the final result had many advantages:  
 * If could help reduce the size the entire build takes on disk. If the same mock library is shared by multiple unit tests, all unit tests will use a single instance of the same DLL.