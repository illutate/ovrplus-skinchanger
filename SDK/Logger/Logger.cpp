#include "Logger.h"

#include "SDK/Files/Paths.h"

#include <windows.h>

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace
{
    constexpr const char* LogFileName = "debug.log";

    HANDLE LogFile = INVALID_HANDLE_VALUE;
    HANDLE ConsoleOut = INVALID_HANDLE_VALUE;
    LONG ReportedExceptions = 0;

    bool IsFatal(DWORD Code)
    {
        switch (Code)
        {
        case EXCEPTION_ACCESS_VIOLATION:
        case EXCEPTION_ILLEGAL_INSTRUCTION:
        case EXCEPTION_PRIV_INSTRUCTION:
        case EXCEPTION_STACK_OVERFLOW:
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        case EXCEPTION_IN_PAGE_ERROR:
            return true;
        default:
            return false;
        }
    }

    void DescribeAddress(std::uintptr_t Address, char* Out, std::size_t Size)
    {
        HMODULE Module = nullptr;
        if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                reinterpret_cast<LPCSTR>(Address), &Module))
        {
            std::snprintf(Out, Size, "%#llx", static_cast<unsigned long long>(Address));
            return;
        }

        char Path[MAX_PATH] = {};
        GetModuleFileNameA(Module, Path, MAX_PATH);
        const char* Name = std::strrchr(Path, '\\');
        Name = Name ? Name + 1 : Path;
        std::snprintf(Out, Size, "%s+%#llx", Name, static_cast<unsigned long long>(Address - reinterpret_cast<std::uintptr_t>(Module)));
    }

    LONG CALLBACK ExceptionHandler(EXCEPTION_POINTERS* Info)
    {
        const EXCEPTION_RECORD* Record = Info->ExceptionRecord;
        if (!IsFatal(Record->ExceptionCode) || InterlockedIncrement(&ReportedExceptions) > 10)
            return EXCEPTION_CONTINUE_SEARCH;

        char Location[MAX_PATH + 32];
        DescribeAddress(reinterpret_cast<std::uintptr_t>(Record->ExceptionAddress), Location, sizeof(Location));

        if (Record->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && Record->NumberParameters >= 2)
        {
            LogE("CRASH / ACCESS VIOLATION: %#lx at %s, attempted %s %#llx",
                 Record->ExceptionCode,
                 Location,
                 Record->ExceptionInformation[0] ? "write" : "read",
                 static_cast<unsigned long long>(Record->ExceptionInformation[1]));
        }
        else
        {
            LogE("CRASH / EXCEPTION: %#lx at %s", Record->ExceptionCode, Location);
        }

        if (LogFile != INVALID_HANDLE_VALUE)
            FlushFileBuffers(LogFile);

        return EXCEPTION_CONTINUE_SEARCH;
    }
}

bool Logger::Init()
{
    // Debug console allocation removed per user request (logs routed to debug.log)
    ConsoleOut = INVALID_HANDLE_VALUE;

    // 2. Open debug.log in the same directory as the DLL
    std::string LogFilePath = Paths::Resolve(LogFileName);
    LogFile = CreateFileA(LogFilePath.c_str(),
                          FILE_APPEND_DATA,
                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                          nullptr,
                          OPEN_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL,
                          nullptr);

    if (LogFile == INVALID_HANDLE_VALUE)
        return false;

    // Vectored exception handler for crash capture
    AddVectoredExceptionHandler(1, ExceptionHandler);

    return true;
}

void Logger::Write(const char* Prefix, const char* Format, ...)
{
    char Message[4096];
    va_list Args;
    va_start(Args, Format);
    int Written = std::vsnprintf(Message, sizeof(Message), Format, Args);
    va_end(Args);

    if (Written < 0)
        return;

    // File output to debug.log
    if (LogFile != INVALID_HANDLE_VALUE)
    {
        char FileBuffer[4196];
        int Total = std::snprintf(FileBuffer, sizeof(FileBuffer), "%s %s\r\n", Prefix, Message);
        if (Total > 0)
        {
            DWORD BytesWritten = 0;
            WriteFile(LogFile, FileBuffer, static_cast<DWORD>(Total), &BytesWritten, nullptr);
            // Flush only on fatal errors
            if (Prefix[1] == '!')
                FlushFileBuffers(LogFile);
        }
    }
}
