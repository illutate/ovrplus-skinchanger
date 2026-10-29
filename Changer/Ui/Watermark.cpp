#include "Watermark.h"

#include "SDK/GameRules/C_DOTAGamerules.h"
#include "SDK/Logger/Logger.h"
#include "SDK/Panorama/Panel.h"
#include "SDK/Panorama/Panorama.h"

#include <cstdio>
#include <string>

namespace
{
    constexpr const char* ContainerId = "OvrServerIndicator";
    constexpr const char* ServerTextId = "OvrServerText";

    std::uint32_t CurrentRegion = 0;
    std::uint32_t UpdateCounter = 0;

    const char* ResolveRegionName(std::uint32_t Region)
    {
        switch (Region)
        {
        case 1:  return "US West";
        case 2:  return "US East";
        case 3:  return "Luxembourg";
        case 4:  return "South Korea";
        case 5:  return "Singapore";
        case 6:  return "Dubai";
        case 7:  return "Australia";
        case 8:  return "Stockholm";
        case 9:  return "Austria";
        case 10: return "Brazil";
        case 11: return "South Africa";
        case 12: return "China Telecom";
        case 13: return "China Unicom";
        case 14: return "Chile";
        case 15: return "Peru";
        case 16: return "India";
        case 17: return "Tokyo, Japan";
        case 19: return "Frankfurt, Germany";
        case 20: return "Moscow, Russia";
        case 25: return "Argentina";
        default: return C_DOTAGamerules::IsInGame() ? "Stockholm" : "Menu / Lobby";
        }
    }

    void BuildIndicator(const Panel& Root)
    {
        // Compact label situated directly under Dota's native top-right net stats (FPS/Ping)
        Panel Container = Panel::Create("Panel", Root, ContainerId);
        Container.SetStyle({
            { "flow-children", "right" },
            { "horizontal-align", "right" },
            { "vertical-align", "top" },
            { "margin-top", "24px" },
            { "margin-right", "16px" },
            { "padding", "2px 8px" },
            { "border-radius", "4px" },
            { "background-color", "rgba(5, 5, 8, 0.65)" },
            { "ui-blur", "gaussian(4px)" },
            { "border", "1px solid rgba(255, 42, 75, 0.40)" },
            { "box-shadow", "0px 0px 8px 1px rgba(255, 30, 60, 0.20)" },
            { "z-index", "10000" }
        });

        // Small stylish accent dot
        Panel Dot = Panel::Create("Label", Container, "OvrServerDot");
        Dot.SetText("● ");
        Dot.SetStyle({
            { "color", "#ff2a4b" },
            { "font-size", "9px" },
            { "vertical-align", "center" },
            { "margin-right", "2px" }
        });
        Dot.Release();

        // Server text
        Panel ServerLabel = Panel::Create("Label", Container, ServerTextId);
        ServerLabel.SetStyle({
            { "color", "#e2e2e2" },
            { "font-size", "11px" },
            { "font-weight", "semi-bold" },
            { "vertical-align", "center" },
            { "letter-spacing", "0.5px" },
            { "white-space", "nowrap" }
        });
        ServerLabel.Release();

        Container.Release();
    }
}

void Watermark::SetServerRegion(std::uint32_t Region)
{
    CurrentRegion = Region;
}

void Watermark::Update()
{
    // Throttled to every 30 frames to guarantee minimal overhead
    if (++UpdateCounter % 30 != 0)
        return;

    Panorama::Scope Scope;
    if (!Scope.IsValid())
        return;

    Panel Root = Panel::Root();
    if (!Root.Handle())
        return;

    Panel Container = Root.FindChild(ContainerId);
    if (!Container.Handle())
    {
        BuildIndicator(Root);
        Container = Root.FindChild(ContainerId);
    }

    if (Container.Handle())
    {
        const char* ServerStr = ResolveRegionName(CurrentRegion);
        char Buffer[128];
        if (C_DOTAGamerules::IsInGame())
            std::snprintf(Buffer, sizeof(Buffer), "Server: %s", ServerStr);
        else
            std::snprintf(Buffer, sizeof(Buffer), "Region: %s", ServerStr);

        Panel Text = Container.FindChild(ServerTextId);
        if (Text.Handle())
        {
            Text.SetText(Buffer);
            Text.Release();
        }

        Container.Release();
    }

    Root.Release();
}
