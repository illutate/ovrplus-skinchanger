#include "Config.h"

#include "SDK/Files/Paths.h"
#include "SDK/Logger/Logger.h"

#include <windows.h>
#include <algorithm>
#include <fstream>
#include <map>
#include <string>
#include <vector>

namespace
{
    constexpr const char* ConfigFileName = "ovr_skins.cfg";

    std::map<std::string, std::string> Values;
    std::string ActiveConfigPath;

    std::string GetExeDir()
    {
        char Buffer[MAX_PATH] = {};
        if (GetModuleFileNameA(nullptr, Buffer, MAX_PATH))
        {
            std::string Dir(Buffer);
            std::size_t Slash = Dir.find_last_of("\\/");
            if (Slash != std::string::npos)
                return Dir.substr(0, Slash + 1);
        }
        return std::string();
    }

    std::vector<std::string> GetCandidatePaths()
    {
        std::vector<std::string> Candidates;

        // 1. Module directory resolved via GetModuleHandleEx
        std::string DllResolved = Paths::Resolve(ConfigFileName);
        if (!DllResolved.empty())
            Candidates.push_back(DllResolved);

        // 2. Dota 2 game win64 directory (e.g. steamapps/common/dota 2 beta/game/bin/win64/)
        std::string ExeDir = GetExeDir();
        if (!ExeDir.empty())
            Candidates.push_back(ExeDir + ConfigFileName);

        // 3. User Cheats directories
        Candidates.push_back("D:\\utilities\\scripts\\cheats\\dota 2\\debug ovrplus 1.03\\" + std::string(ConfigFileName));
        Candidates.push_back("D:\\utilities\\scripts\\cheats\\dota 2\\" + std::string(ConfigFileName));

        // Deduplicate
        std::vector<std::string> Unique;
        for (const auto& P : Candidates)
        {
            if (!P.empty() && std::find(Unique.begin(), Unique.end(), P) == Unique.end())
                Unique.push_back(P);
        }
        return Unique;
    }

    void WriteToFile(const std::string& Path)
    {
        if (Path.empty())
            return;
        std::ofstream File(Path, std::ios::trunc);
        if (!File)
            return;
        for (const auto& [Key, Value] : Values)
            File << Key << '=' << Value << '\n';
    }

    void Save()
    {
        if (ActiveConfigPath.empty())
            ActiveConfigPath = Paths::Resolve(ConfigFileName);

        // Write to active path
        WriteToFile(ActiveConfigPath);

        // Also sync to Dota win64 folder if different
        std::string ExeDir = GetExeDir();
        if (!ExeDir.empty())
        {
            std::string Win64Config = ExeDir + ConfigFileName;
            if (Win64Config != ActiveConfigPath)
                WriteToFile(Win64Config);
        }
    }
}

bool Config::Init()
{
    Values.clear();
    ActiveConfigPath.clear();

    auto Candidates = GetCandidatePaths();
    for (const auto& Path : Candidates)
    {
        std::ifstream File(Path);
        if (!File)
            continue;

        std::string Line;
        while (std::getline(File, Line))
        {
            if (!Line.empty() && Line.back() == '\r')
                Line.pop_back();

            std::size_t Separator = Line.find('=');
            if (Separator == std::string::npos || Separator == 0)
                continue;
            Values[Line.substr(0, Separator)] = Line.substr(Separator + 1);
        }

        if (!Values.empty())
        {
            ActiveConfigPath = Path;
            Log("Config %s loaded, %zu keys", Path.c_str(), Values.size());
            return true;
        }
    }

    ActiveConfigPath = Candidates.empty() ? Paths::Resolve(ConfigFileName) : Candidates.front();
    Log("No existing config with data found. Active config target: %s", ActiveConfigPath.c_str());
    return true;
}

std::string Config::Get(const char* Key)
{
    auto Entry = Values.find(Key);
    return Entry != Values.end() ? Entry->second : std::string();
}

void Config::Set(const char* Key, const std::string& Value)
{
    auto Entry = Values.find(Key);
    if (Value.empty())
    {
        if (Entry == Values.end())
            return;
        Values.erase(Entry);
    }
    else
    {
        if (Entry != Values.end() && Entry->second == Value)
            return;
        Values[Key] = Value;
    }
    Save();
}
