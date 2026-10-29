#include "RunFrameHook.h"

#include "Changer/Match/EmblemEffect.h"
#include "Changer/Match/ImpactEffects.h"
#include "Changer/Inventory/InventorySync.h"
#include "Changer/Match/NetSkins.h"
#include "Changer/Ui/TreasureAddButton.h"
#include "Changer/Ui/UnusualSettings.h"
#include "Changer/Ui/Watermark.h"
#include "Hooks/GcRecvHook.h"
#include "Misc/Patterns.h"
#include "SDK/Logger/Logger.h"
#include "SDK/Memory/Memory.h"
#include "SDK/Optimization/ProcessOptimizer.h"

#include <MinHook.h>

#include <cstdint>

namespace
{
    void (*Original)(void*) = nullptr;
    std::uint32_t FrameCounter = 0;

    void Detour(void* Engine)
    {
        ProcessOptimizer::Update();
        InventorySync::Update();
        GcRecvHook::Update();
        Original(Engine);

        NetSkins::Update();
        EmblemEffect::Update();
        ImpactEffects::Update();
        Watermark::Update();

        // Throttle UI traversal to reduce Panorama DOM walk overhead (runs every 30 frames ~300-400ms)
        if (++FrameCounter % 30 == 0)
        {
            UnusualSettings::Update();
            TreasureAddButton::Update();
        }
    }
}

bool RunFrameHook::Install()
{
    std::uintptr_t Target = Memory::FindPattern("panorama.dll", Patterns::RunFrame);
    if (!Target)
    {
        LogE("RunFrame pattern not found");
        return false;
    }

    MH_STATUS Status = MH_Initialize();
    if (Status != MH_OK && Status != MH_ERROR_ALREADY_INITIALIZED)
    {
        LogE("MinHook init failed: %s", MH_StatusToString(Status));
        return false;
    }

    auto* TargetFn = reinterpret_cast<void*>(Target);
    Status = MH_CreateHook(TargetFn, reinterpret_cast<void*>(Detour), reinterpret_cast<void**>(&Original));
    if (Status == MH_OK)
        Status = MH_EnableHook(TargetFn);

    if (Status != MH_OK)
    {
        LogE("RunFrame hook failed: %s", MH_StatusToString(Status));
        return false;
    }

    Log("RunFrame %#llx", static_cast<unsigned long long>(Target));
    return true;
}
