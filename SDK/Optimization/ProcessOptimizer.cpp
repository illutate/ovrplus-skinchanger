#include "ProcessOptimizer.h"

#include "SDK/GameRules/C_DOTAGamerules.h"
#include "SDK/Logger/Logger.h"

#include <windows.h>
#include <timeapi.h>

#include <chrono>

#ifndef PROCESS_POWER_THROTTLING_CURRENT_VERSION
#define PROCESS_POWER_THROTTLING_CURRENT_VERSION 1
#define PROCESS_POWER_THROTTLING_EXECUTION_SPEED 0x1
typedef struct _PROCESS_POWER_THROTTLING_STATE {
    ULONG Version;
    ULONG ControlMask;
    ULONG StateMask;
} PROCESS_POWER_THROTTLING_STATE, *PPROCESS_POWER_THROTTLING_STATE;
#endif

#ifndef ProcessPowerThrottling
#define ProcessPowerThrottling ((PROCESS_INFORMATION_CLASS)4)
#endif

namespace
{
    void CleanGameCache()
    {
        // 1. Trim unused physical memory pages and purge inactive texture/sound caches
        using FnEmptyWorkingSet = BOOL(WINAPI*)(HANDLE);
        auto pEmptyWorkingSet = reinterpret_cast<FnEmptyWorkingSet>(
            GetProcAddress(GetModuleHandleA("kernel32.dll"), "K32EmptyWorkingSet"));
        if (!pEmptyWorkingSet)
            pEmptyWorkingSet = reinterpret_cast<FnEmptyWorkingSet>(
                GetProcAddress(LoadLibraryA("psapi.dll"), "EmptyWorkingSet"));
        if (pEmptyWorkingSet)
            pEmptyWorkingSet(GetCurrentProcess());

        // 2. Compact default process heap
        HeapCompact(GetProcessHeap(), 0);

        Log("In-game cache cleaned (5-minute interval)");
    }
}

void ProcessOptimizer::ApplySystemTweaks()
{
    // 1. High CPU priority class for dota2.exe (all render, game and worker threads)
    if (SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
        Log("Process priority set to HIGH_PRIORITY_CLASS");

    // 2. High precision 1ms multimedia timer resolution
    timeBeginPeriod(1);

    // 3. Disable Windows 11 EcoQoS / Power Throttling to keep all performance cores at max clock
    using FnSetProcessInformation = BOOL(WINAPI*)(HANDLE, PROCESS_INFORMATION_CLASS, LPVOID, DWORD);
    auto pSetProcessInformation = reinterpret_cast<FnSetProcessInformation>(
        GetProcAddress(GetModuleHandleA("kernel32.dll"), "SetProcessInformation"));
    if (pSetProcessInformation)
    {
        PROCESS_POWER_THROTTLING_STATE PowerThrottling = {};
        PowerThrottling.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
        PowerThrottling.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;
        PowerThrottling.StateMask = 0;
        pSetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling, &PowerThrottling, sizeof(PowerThrottling));
    }

    // 4. Multimedia Class Scheduler Service (MMCSS) for gaming latency
    HMODULE Avrt = LoadLibraryA("avrt.dll");
    if (Avrt)
    {
        using FnAvSetMmThreadCharacteristicsA = HANDLE(WINAPI*)(LPCSTR, LPDWORD);
        auto pAvSetMm = reinterpret_cast<FnAvSetMmThreadCharacteristicsA>(
            GetProcAddress(Avrt, "AvSetMmThreadCharacteristicsA"));
        if (pAvSetMm)
        {
            DWORD TaskIndex = 0;
            if (pAvSetMm("Games", &TaskIndex))
                Log("MMCSS registered for 'Games' profile");
        }
    }

    // 5. Prevent thread suspension / idle power throttling
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);

    Log("System performance tweaks applied successfully");
}

void ProcessOptimizer::Update()
{
    static std::uint32_t FrameTicks = 0;
    // Check once every ~120 frames (~1-2 seconds)
    if (++FrameTicks % 120 != 0)
        return;

    if (!C_DOTAGamerules::IsInGame())
        return;

    static auto LastPurgeTime = std::chrono::steady_clock::now();
    auto Now = std::chrono::steady_clock::now();
    auto ElapsedMinutes = std::chrono::duration_cast<std::chrono::minutes>(Now - LastPurgeTime).count();

    if (ElapsedMinutes >= 5)
    {
        LastPurgeTime = Now;
        CleanGameCache();
    }
}
