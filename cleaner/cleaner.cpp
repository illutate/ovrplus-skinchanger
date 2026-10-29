#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <shlobj.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace
{
    // Target files that OVR SkinChanger creates
    const std::vector<std::string> TargetFiles = {
        "ovr_skins.cfg",
        "ovr_skins.log",
        "debug.log",
        "netdelta_dump.bin",
        "packet_dump.bin",
        "panorama_dump.txt",
        "v8_dump.txt",
        "patterns_dump.txt"
    };

    void SetConsoleColor(WORD color)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
    }

    void PrintBanner()
    {
        SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN);
        std::cout << "=================================================================\n";
        std::cout << "             MAYDEVILCRY - STANDALONE CLEANER                    \n";
        std::cout << "=================================================================\n\n";
        SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

    bool KillProcessByName(const wchar_t* processName)
    {
        bool found = false;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE)
            return false;

        PROCESSENTRY32W entry;
        entry.dwSize = sizeof(entry);

        if (Process32FirstW(snapshot, &entry))
        {
            do
            {
                if (_wcsicmp(entry.szExeFile, processName) == 0)
                {
                    found = true;
                    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
                    if (hProcess)
                    {
                        TerminateProcess(hProcess, 0);
                        CloseHandle(hProcess);
                        SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
                        std::wcout << L"[*] Closed running process: " << processName << L" (PID: " << entry.th32ProcessID << L")\n";
                        SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                    }
                }
            } while (Process32NextW(snapshot, &entry));
        }

        CloseHandle(snapshot);
        return found;
    }

    std::string ReadRegistryString(HKEY root, const char* subkey, const char* valueName)
    {
        HKEY hKey = nullptr;
        if (RegOpenKeyExA(root, subkey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
            return "";

        char buffer[1024] = {};
        DWORD bufferSize = sizeof(buffer);
        DWORD type = 0;
        std::string result;

        if (RegQueryValueExA(hKey, valueName, nullptr, &type, reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS)
        {
            if (type == REG_SZ || type == REG_EXPAND_SZ)
                result = buffer;
        }

        RegCloseKey(hKey);
        return result;
    }

    std::vector<fs::path> GetSteamLibraryPaths(const fs::path& steamPath)
    {
        std::vector<fs::path> libraries;
        if (steamPath.empty() || !fs::exists(steamPath))
            return libraries;

        libraries.push_back(steamPath);

        fs::path vdfPath = steamPath / "steamapps" / "libraryfolders.vdf";
        if (!fs::exists(vdfPath))
            return libraries;

        std::ifstream file(vdfPath);
        if (!file.is_open())
            return libraries;

        std::string line;
        while (std::getline(file, line))
        {
            std::size_t pathKey = line.find("\"path\"");
            if (pathKey != std::string::npos)
            {
                std::size_t firstQuote = line.find('"', pathKey + 6);
                if (firstQuote != std::string::npos)
                {
                    std::size_t secondQuote = line.find('"', firstQuote + 1);
                    if (secondQuote != std::string::npos)
                    {
                        std::string lib = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
                        std::string normalized;
                        for (std::size_t i = 0; i < lib.size(); ++i)
                        {
                            if (lib[i] == '\\' && i + 1 < lib.size() && lib[i + 1] == '\\')
                            {
                                normalized.push_back('\\');
                                ++i;
                            }
                            else
                            {
                                normalized.push_back(lib[i]);
                            }
                        }
                        fs::path p(normalized);
                        if (fs::exists(p))
                            libraries.push_back(p);
                    }
                }
            }
        }

        return libraries;
    }

    std::vector<fs::path> CollectSearchPaths()
    {
        std::vector<fs::path> paths;

        // 1. Current executable directory
        char exePath[MAX_PATH] = {};
        if (GetModuleFileNameA(nullptr, exePath, MAX_PATH))
        {
            fs::path currentDir = fs::path(exePath).parent_path();
            paths.push_back(currentDir);
            if (currentDir.has_parent_path())
                paths.push_back(currentDir.parent_path());
        }

        // 2. Current working directory
        try {
            paths.push_back(fs::current_path());
        } catch (...) {}

        // 3. User Desktop
        char desktopPath[MAX_PATH] = {};
        if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, 0, desktopPath)))
            paths.push_back(fs::path(desktopPath));

        fs::path manualDesktop("C:\\Users\\project_320\\Desktop");
        if (fs::exists(manualDesktop))
            paths.push_back(manualDesktop);

        // 4. Cheats directories
        fs::path cheatRoot("D:\\utilities\\scripts\\cheats\\dota 2");
        if (fs::exists(cheatRoot))
        {
            paths.push_back(cheatRoot);
            paths.push_back(cheatRoot / "debug ovrplus 1.03");
            paths.push_back(cheatRoot / "debug ovrplus 1.02");
            paths.push_back(cheatRoot / "release ovrplus 1.0");
        }

        // 5. Workspaces
        fs::path proj("D:\\ovr-skinchanger");
        if (fs::exists(proj))
        {
            paths.push_back(proj);
            paths.push_back(proj / "Debug-ovr-plus");
        }
        fs::path projectPath("D:\\dota 2 volumuer");
        if (fs::exists(projectPath))
            paths.push_back(projectPath);

        // 6. Steam & Dota 2 paths
        std::string steamPathStr = ReadRegistryString(HKEY_CURRENT_USER, "Software\\Valve\\Steam", "SteamPath");
        if (steamPathStr.empty())
            steamPathStr = ReadRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Valve\\Steam", "InstallPath");

        if (!steamPathStr.empty())
        {
            fs::path steamPath(steamPathStr);
            auto libs = GetSteamLibraryPaths(steamPath);
            for (const auto& lib : libs)
            {
                fs::path dotaBeta = lib / "steamapps" / "common" / "dota 2 beta";
                fs::path dotaGame = dotaBeta / "game";
                if (fs::exists(dotaGame))
                {
                    paths.push_back(dotaGame / "bin" / "win64");
                    paths.push_back(dotaGame / "dota" / "bin" / "win64");
                    paths.push_back(dotaGame / "dota");
                    paths.push_back(dotaGame);
                    paths.push_back(dotaBeta);
                }
            }
        }

        // Remove duplicates
        std::vector<fs::path> uniquePaths;
        for (const auto& p : paths)
        {
            std::error_code ec;
            if (fs::exists(p, ec))
            {
                fs::path can = fs::canonical(p, ec);
                if (!ec && std::find(uniquePaths.begin(), uniquePaths.end(), can) == uniquePaths.end())
                    uniquePaths.push_back(can);
            }
        }

        return uniquePaths;
    }
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    PrintBanner();

    std::cout << "[*] Checking for running instances of Dota 2...\n";
    if (KillProcessByName(L"dota2.exe"))
    {
        std::cout << "[*] Waiting for process termination to release file locks...\n";
        Sleep(1000);
    }
    else
    {
        std::cout << "[+] Dota 2 is not running.\n";
    }

    std::cout << "\n[*] Scanning target locations for MayDevilCry configurations and cache...\n\n";

    auto searchPaths = CollectSearchPaths();
    std::size_t totalDeleted = 0;
    std::uintmax_t totalBytes = 0;

    for (const auto& dir : searchPaths)
    {
        for (const auto& filename : TargetFiles)
        {
            fs::path target = dir / filename;
            std::error_code ec;
            if (fs::exists(target, ec) && !fs::is_directory(target, ec))
            {
                std::uintmax_t fileSize = fs::file_size(target, ec);
                if (fs::remove(target, ec))
                {
                    ++totalDeleted;
                    totalBytes += fileSize;
                    SetConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                    std::cout << "  [DELETED] ";
                    SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                    std::cout << target.string() << " (" << fileSize << " bytes)\n";
                }
                else
                {
                    SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
                    std::cout << "  [FAILED]  ";
                    SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                    std::cout << target.string() << " (error: " << ec.message() << ")\n";
                }
            }
        }
    }

    std::cout << "\n=================================================================\n";
    if (totalDeleted > 0)
    {
        SetConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << "[SUCCESS] Cleanup completed!\n";
        SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        std::cout << "          Files removed:  " << totalDeleted << "\n";
        std::cout << "          Disk reclaimed: " << totalBytes << " bytes (" << (totalBytes / 1024.0) << " KB)\n\n";
        std::cout << "[INFO] All unlocked skins, gems, unusual effects and custom loadouts\n";
        std::cout << "       have been permanently purged. Your game state is 100% clean.\n";
    }
    else
    {
        SetConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << "[CLEAN] No MayDevilCry skin files or artifacts were found.\n";
        SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        std::cout << "        Your game and directory are already completely clean.\n";
    }
    std::cout << "=================================================================\n\n";

    std::cout << "Press any key to exit...";
    std::cin.get();
    return 0;
}
