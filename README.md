<div align="center">

# ⚡ OVR SkinChanger for Dota 2 — v1.0 Release

**High-performance, internal cosmetic and inventory skin changer for Dota 2 (Source 2 / 64-bit Windows).**

[![C++20](https://img.shields.io/badge/Language-C%2B%2B20-blue.svg?style=for-the-badge&logo=cplusplus)](https://en.wikipedia.org/wiki/C%2B%2B20)
[![Platform](https://img.shields.io/badge/Platform-Windows%20x64-0078D6.svg?style=for-the-badge&logo=windows)](https://microsoft.com)
[![Engine](https://img.shields.io/badge/Engine-Valve%20Source%202-red.svg?style=for-the-badge&logo=steam)](https://developer.valvesoftware.com/wiki/Source_2)
[![Release](https://img.shields.io/badge/Release-v1.0-brightgreen.svg?style=for-the-badge)](https://github.com)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-green.svg?style=for-the-badge)](LICENSE)

---

[Features](#-key-features) • [Changelog v1.0](#-whats-new-in-v10-changelog) • [Building](#-building-from-source) • [Installation & Injection](#-installation--injection) • [Cleaner Tool](#-deep-cleaner-cleanerexe) • [License](#-license)

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

## 📦 What's New in v1.0 (Changelog)

Starting from the initial public base repository, the following critical improvements, missing components, and stability fixes have been implemented:

### 1. 🛠️ Complete Self-Contained Build Environment
* **Resolved Missing Protobuf Dependencies**: The original repository had `/src/Resource/Protobufs/` in `.gitignore`, causing fatal compilation errors due to missing Google Protobuf runtime and Valve Dota 2 `.pb.h` / `.pb.cc` files.
* **Pre-generated Dota 2 Protobuf Schemas**: Added full compilation for all 18 Valve network and GameCoordinator protocols (`base_gcmessages`, `dota_usermessages`, `econ_gcmessages`, `gcsdk_gcmessages`, `netmessages`, etc.).
* **Bundled Static Protobuf Runtime**: Integrated Google Protobuf v3.21.12 C++ runtime (`libprotobuf.a` + headers) directly into `Resource/`, making the project 100% buildable without external SDK setups.

### 2. ⚡ Standalone Static Linking
* Added `-static -static-libgcc -static-libstdc++` linker configuration.
* Removed external runtime dependencies (`libwinpthread-1.dll`, `msvcp140.dll`, etc.). The resulting DLL is entirely self-sufficient and guaranteed not to throw `STATUS_DLL_NOT_FOUND` (0xC0000135) upon injection.

### 3. 🧹 Deep Cleaner Tool (`cleaner.exe`)
* Implemented a dedicated standalone utility to solve persistent skins and game state corruption:
  * Detects and terminates running `dota2.exe` processes to release file locks.
  * Discovers Steam library folders across all drives (`C:\`, `D:\`, etc.).
  * Wipes `ovr_skins.cfg`, `debug.log`, `netdelta_dump.bin`, `packet_dump.bin`, and Panorama cache files.

### 4. 🔨 Streamlined Build System
* Added cross-compatible [`CMakeLists.txt`](CMakeLists.txt) supporting CMake 3.20+ and Ninja.
* Added one-click [`build.bat`](build.bat) script: builds both `cleaner.exe` and `ovr-skinchanger.dll` on 12 threads with automatic symbol stripping.

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
6. The **OVR Debug Console** will appear, displaying hook installation status.
7. Open the hero loadout or store in-game: click `+` on any item to grant and equip it!

---

## 🧹 Deep Cleaner (`cleaner.exe`)

To revert all cosmetic modifications back to stock:
1. Run `cleaner.exe`.
2. The cleaner will close Dota 2, purge all custom configs and dumps, and restore default game state.

---

## 📜 License

This project is licensed under the terms of the **GNU General Public License v3.0** (GPL-3.0).
See [LICENSE](LICENSE) for full terms.
