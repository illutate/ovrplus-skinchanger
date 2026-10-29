<div align="center">

# ⚡ MayDevilCry SkinChanger for Dota 2

**High-performance, internal cosmetic and inventory skin changer for Dota 2 (Source 2 / 64-bit Windows).**

[![Version](https://img.shields.io/badge/Version-1.04-orange.svg?style=for-the-badge)](https://github.com/illutate/ovrplus-skinchanger/releases)
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
  - Native socketing support: add sockets, insert ethereal gems and customize prismatic colors directly in the socket dialog.
  - All rare, event, and hidden unusual effects unlocked (Frostbloom, Ghostly Presence, Sunbreeze, Scorching Soles, Featherfall, Ethereal Flame, Bleak Hallucination, and more).
  - Dedicated unusual effect selector (`Unusual Settings`) built into the item details page.

- **⚡ In-Game Performance & Cache Optimization**
  - **Automated 5-minute cache purge:** Periodically trims inactive texture buffers and defragments process memory while in-game (Turbo, All Pick, Ranked).
  - **Kernel-level priority:** Sets Dota 2 to `HIGH_PRIORITY_CLASS` across all threads.
  - **Windows MMCSS Gaming Mode:** Registers Dota 2 threads in Windows Multimedia Class Scheduler for ultra-low latency.
  - **High-Precision Timer:** Activates 1ms system multimedia timer resolution (`timeBeginPeriod(1)`).
  - **Power Throttling Bypass:** Disables Windows 11 EcoQoS to keep all performance cores at peak clocks.

- **🌐 Sleek Native Server Region Indicator**
  - Unobtrusive, elegant indicator situated right below Dota's native top-right net stats (FPS/Ping).
  - Displays the active match server region in English (Frankfurt, Stockholm, Luxembourg, Moscow, etc.) without requiring the performance-heavy Steam overlay.

- **🧹 One-Click Deep Cleaner (`cleaner.exe`)**
  - Standalone utility that instantly wipes all saved configurations, unusual effect overrides, and cache files across all directories.
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
2. `maydevilcry.dll` via CMake + Ninja with multi-threading (`-j 12`) and static runtime
3. Automatically strips unneeded debug symbols

### Manual Build via CMake
```cmd
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 12
strip --strip-unneeded build/maydevilcry.dll
```

---

## 🚀 Installation & Injection

1. **Launch Dota 2** (64-bit client).
2. Open any standard 64-bit injector (e.g. *Extreme Injector v3*).
3. Select `dota2.exe` as the target process.
4. Set injection method to **Standard Injection** or **Manual Map**.
5. Select `maydevilcry.dll` and click **Inject**.
6. Open the hero loadout or store in-game: click `+` on any item to grant and equip it!

---

## 🧹 Deep Cleaner (`cleaner.exe`)

To revert all cosmetic modifications back to stock:
1. Run `cleaner.exe`.
2. The cleaner will close Dota 2, purge all custom configs and dumps, and restore default game state.

---

## 👥 Credits

* **Original Base Author**: [`pastow`](https://github.com/pastow) — author and creator of the original internal skinchanger foundation.
* **MayDevilCry**: Performance optimization suite, automated in-game cache purger, full unusual socketing unlock, native server region HUD, deep cleaner, and static runtime build.

---

## 📄 License

This project is licensed under the GPL-3.0 License - see the [LICENSE](LICENSE) file for details.
