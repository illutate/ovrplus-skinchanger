#include "ISteamUser.h"

#include "SDK/Logger/Logger.h"

#include <windows.h>

namespace
{
    using SteamUserFn = ISteamUser* (*)();
    using GetSteamIdFn = std::uint64_t (*)(ISteamUser*);
    using SteamFriendsFn = void* (*)();
    using GetPersonaNameFn = const char* (*)(void*);

    SteamUserFn  SteamUser = nullptr;
    GetSteamIdFn GetSteamIdExport = nullptr;
    SteamFriendsFn SteamFriends = nullptr;
    GetPersonaNameFn GetPersonaNameExport = nullptr;
}

bool ISteamUser::Init()
{
    HMODULE Api = GetModuleHandleA("steam_api64.dll");
    if (!Api)
    {
        LogE("steam_api64.dll not loaded");
        return false;
    }

    SteamUser = reinterpret_cast<SteamUserFn>(GetProcAddress(Api, "SteamAPI_SteamUser_v023"));
    if (!SteamUser)
        SteamUser = reinterpret_cast<SteamUserFn>(GetProcAddress(Api, "SteamAPI_SteamUser_v021"));
    if (!SteamUser)
        SteamUser = reinterpret_cast<SteamUserFn>(GetProcAddress(Api, "SteamUser"));

    GetSteamIdExport = reinterpret_cast<GetSteamIdFn>(GetProcAddress(Api, "SteamAPI_ISteamUser_GetSteamID"));

    SteamFriends = reinterpret_cast<SteamFriendsFn>(GetProcAddress(Api, "SteamAPI_SteamFriends_v018"));
    if (!SteamFriends)
        SteamFriends = reinterpret_cast<SteamFriendsFn>(GetProcAddress(Api, "SteamAPI_SteamFriends_v017"));
    if (!SteamFriends)
        SteamFriends = reinterpret_cast<SteamFriendsFn>(GetProcAddress(Api, "SteamFriends"));

    GetPersonaNameExport = reinterpret_cast<GetPersonaNameFn>(GetProcAddress(Api, "SteamAPI_ISteamFriends_GetPersonaName"));

    if (!SteamUser || !GetSteamIdExport)
    {
        LogE("steam_api exports missing: SteamUser %p GetSteamID %p", reinterpret_cast<void*>(SteamUser),
             reinterpret_cast<void*>(GetSteamIdExport));
        return false;
    }

    Log("ISteamUser %p SteamFriends %p", static_cast<void*>(Get()), reinterpret_cast<void*>(SteamFriends));
    return true;
}

ISteamUser* ISteamUser::Get()
{
    return SteamUser ? SteamUser() : nullptr;
}

std::uint64_t ISteamUser::GetSteamID()
{
    return GetSteamIdExport ? GetSteamIdExport(this) : 0;
}

std::uint32_t ISteamUser::AccountId()
{
    return static_cast<std::uint32_t>(GetSteamID() & 0xFFFFFFFFull);
}

const char* ISteamUser::PersonaName()
{
    if (GetPersonaNameExport && SteamFriends)
    {
        void* Friends = SteamFriends();
        if (Friends)
        {
            const char* Name = GetPersonaNameExport(Friends);
            if (Name && *Name)
                return Name;
        }
    }
    return "Player";
}
