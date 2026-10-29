#include "Paths.h"

#include <windows.h>

std::string Paths::Resolve(const char* FileName)
{
    static const std::string Directory = []() {
        HMODULE Module = nullptr;
        char Path[MAX_PATH] = {};
        if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                               reinterpret_cast<LPCSTR>(&Paths::Resolve), &Module) &&
            GetModuleFileNameA(Module, Path, MAX_PATH))
        {
            std::string Dir(Path);
            std::size_t Slash = Dir.find_last_of("\\/");
            if (Slash != std::string::npos)
                return Dir.substr(0, Slash + 1);
        }
        return std::string();
    }();

    return Directory.empty() ? std::string(FileName) : Directory + FileName;
}
