#include "FakeLoadout.h"

#include "Changer/Inventory/FakeItems.h"
#include "SDK/Econ/CEconItemSchema.h"

#include <chrono>
#include <mutex>

namespace
{
    constexpr std::uint32_t StyleUnset = 0xFF;

    struct Equipped
    {
        CEconItemDefinition* Definition;
        int Style;
    };

    std::mutex CacheMutex;
    std::vector<Equipped> CachedDefinitions;
    std::vector<CEconItemDefinition::AssetModifier> CachedModifiers;
    std::vector<std::string> CachedModels;
    std::chrono::steady_clock::time_point LastCacheUpdate;
    constexpr auto CacheTtl = std::chrono::milliseconds(250);

    void RefreshCacheIfNeeded()
    {
        auto now = std::chrono::steady_clock::now();
        if (!CachedDefinitions.empty() && (now - LastCacheUpdate) < CacheTtl)
            return;

        CEconItemSchema* Schema = CEconItemSchema::Get();
        if (!Schema)
            return;

        std::vector<Equipped> NewDefs;
        FakeItems::ForEach([&](CSOEconItem& Item) {
            if (Item.equipped_state_size() == 0)
                return;
            if (CEconItemDefinition* Def = Schema->GetItemDefinition(Item.def_index()))
                NewDefs.push_back({Def, Item.style() == StyleUnset ? 0 : static_cast<int>(Item.style())});
        });

        std::vector<CEconItemDefinition::AssetModifier> NewModifiers;
        for (const Equipped& Item : NewDefs)
            for (auto& Modifier : Item.Definition->AssetModifiers(Item.Style))
                NewModifiers.push_back(std::move(Modifier));

        std::vector<std::string> NewModels;
        for (const Equipped& Item : NewDefs)
            for (auto& Model : Item.Definition->Models())
                NewModels.push_back(std::move(Model));

        CachedDefinitions = std::move(NewDefs);
        CachedModifiers = std::move(NewModifiers);
        CachedModels = std::move(NewModels);
        LastCacheUpdate = now;
    }
}

std::vector<CEconItemDefinition::AssetModifier> FakeLoadout::EquippedModifiers()
{
    std::lock_guard<std::mutex> Lock(CacheMutex);
    RefreshCacheIfNeeded();
    return CachedModifiers;
}

std::vector<std::string> FakeLoadout::EquippedModels()
{
    std::lock_guard<std::mutex> Lock(CacheMutex);
    RefreshCacheIfNeeded();
    return CachedModels;
}

std::string FakeLoadout::Find(const std::vector<CEconItemDefinition::AssetModifier>& Modifiers, const char* Type, const char* Asset)
{
    for (const auto& Modifier : Modifiers)
        if (Modifier.Type == Type && Modifier.Asset == Asset && !Modifier.Modifier.empty())
            return Modifier.Modifier;
    return {};
}

FakeLoadout::Source FakeLoadout::FindSource(const char* Type, const char* Asset)
{
    std::lock_guard<std::mutex> Lock(CacheMutex);
    RefreshCacheIfNeeded();

    Source Out;
    for (const Equipped& Item : CachedDefinitions)
    {
        std::string Modifier = Find(Item.Definition->AssetModifiers(Item.Style), Type, Asset);
        if (Modifier.empty())
            continue;
        Out.Modifier = std::move(Modifier);
        Out.Definition = Item.Definition;
        Out.Style = Item.Style;
        return Out;
    }
    return Out;
}
