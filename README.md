<div align="center">

# ⚡ OVRPLUS SkinChanger for Dota 2

**High-performance, internal cosmetic and inventory skin changer for Dota 2 (Source 2 / 64-bit Windows).**

[![C++20](https://img.shields.io/badge/Language-C%2B%2B20-blue.svg?style=for-the-badge&logo=cplusplus)](https://en.wikipedia.org/wiki/C%2B%2B20)
[![Platform](https://img.shields.io/badge/Platform-Windows%20x64-0078D6.svg?style=for-the-badge&logo=windows)](https://microsoft.com)
[![Engine](https://img.shields.io/badge/Engine-Valve%20Source%202-red.svg?style=for-the-badge&logo=steam)](https://developer.valvesoftware.com/wiki/Source_2)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-green.svg?style=for-the-badge)](LICENSE)

---

[Features](#-key-features) • [Building](#-building-from-source) • [Installation & Injection](#-installation--injection) • [Cleaner Tool](#-deep-cleaner-cleanerexe) • [Credits](#-credits) • [License](#-license)

</div>

---

## 🌟 Key Features

- **🎮 Full Cosmetic Unlock**
  - Add any item, set, arcana, persona, immortal, courier, or ward directly into your client-side inventory.
  - Seamlessly equip any cosmetic from the native Dota 2 hero loadout.
  - Automatic style unlocks: all alternative styles for arcanas and sets available instantly.

- **💎 Sockets, Prismatic & Ethereal Gems**
  - Native socketing support: add sockets, insert ethereal gems and customize prismatic colors.
  - Dedicated unusual effect selector (`Unusual Settings`) built into the item details page.
  - Full support for event unusual effects, including Frostbloom, Ethereal Flame, Bleak Hallucination, and more.

- **⚡ Zero-Lag High-Performance Architecture**
  - Throttled Panorama UI traversal (runs asynchronously without lowering game framerate).
  - NetDelta schema and entity rewriter for in-game model, material group, and particle replacement.
  - High-efficiency cache for equipped definitions, avoiding unnecessary heap allocations per tick.

- **🖥️ Real-time Interactive Debug Console**
  - Instant console window spawned upon DLL injection.
  - Color-coded logs (`[+]` success, `[!]` warning, `[-]` error) for hooks, schema resolution, and packet events.
  - Crash capture with Vectored Exception Handling, logging faulting addresses and instructions directly to `debug.log`.

- **🧹 One-Click Deep Cleaner (`cleaner.exe`)**
  - Standalone utility that instantly wipes all saved configurations, unusual effect overrides, and cache files.
  - Ensures clean state before client updates or account switching.

---

## 🔨 Building from Source

### Prerequisites
* Windows 10/11 x64
* **MinGW-W64** (GCC 13+ with C++20 support)
* **CMake 3.20+** & **Ninja** (included in standard MinGW WinLibs distributions)

### One-Click Build
Run the build script from the repository root:
```cmd
build.bat
```

This compiles:
1. `cleaner.exe` via GCC with `-O3 -s -static`
2. `ovr-skinchanger.dll` via CMake + Ninja with multi-threading (`-j 12`) and static runtime
3. Automatically strips unneeded debug symbols

### Manual Build via CMake
```cmd
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 12
strip --strip-unneeded build/ovr-skinchanger.dll
```

---

## 🚀 Installation & Injection

1. **Launch Dota 2** (64-bit client).
2. Open any standard 64-bit injector (e.g. *Extreme Injector v3*).
3. Select `dota2.exe` as the target process.
4. Set injection method to **Standard Injection** or **Manual Map**.
5. Select `ovr-skinchanger.dll` and click **Inject**.
6. The **OVRPLUS Debug Console** will appear, displaying hook installation status.
7. Open the hero loadout or store in-game: click `+` on any item to grant and equip it!

---

## 🧹 Deep Cleaner (`cleaner.exe`)

To revert all cosmetic modifications back to stock:
1. Run `cleaner.exe`.
2. The cleaner will close Dota 2, purge all custom configs and dumps, and restore default game state.

---

## 👥 Credits

* **Original Base Author**: [`pastow`](https://github.com/pastow) — author and creator of the original internal skinchanger foundation.
* **OVRPLUS**: Standalone build environment, pre-compiled Valve protobuf definitions, deep cleaner integration, and static runtime build.

---

## 📜 License

This project is licensed under the terms of the **GNU General Public License v3.0** (GPL-3.0).
See [LICENSE](LICENSE) for full terms.
