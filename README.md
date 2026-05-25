# MuCalc

A small VST3 / CLAP / AU plugin that displays BPM-synced note-division times. Drop it on any track to see whole / half / quarter / 8th / 16th / 32nd / 64th notes — straight, dotted, and triplet — converted to milliseconds, Hertz, or samples. Click any cell to copy that value to the clipboard. Useful for setting compressor attack/release, delay times, reverb pre-delay, sidechain release, tempo-synced LFO rates, and so on.

- BPM follows the host project automatically. Toggle "Sync to host" off to type a manual BPM.
- Three controls only: BPM, Sync-to-host, Display-mode. Anything else is computed display data.
- No audio processing — input passes through unchanged so the plugin sits transparently on any track.

## Build prerequisites

### Windows (primary target)
1. **Visual Studio 2022 Community** with the **"Desktop development with C++"** workload — https://visualstudio.microsoft.com/downloads/
2. **CMake 3.22+** — https://cmake.org/download/ (tick "Add CMake to PATH" during install)
3. **Git** (already present on most dev boxes)

### macOS
1. **Xcode 14+** from the App Store, then `xcode-select --install`
2. **CMake 3.22+** via Homebrew: `brew install cmake`

### Linux (Debian / Ubuntu)
```
sudo apt install build-essential cmake git pkg-config \
  libasound2-dev libjack-jackd2-dev libcurl4-openssl-dev \
  libfreetype-dev libfontconfig1-dev \
  libx11-dev libxcomposite-dev libxcursor-dev libxext-dev \
  libxinerama-dev libxrandr-dev libxrender-dev \
  libwebkit2gtk-4.1-dev libglu1-mesa-dev mesa-common-dev
```

## Clone with submodules

```
git clone --recurse-submodules https://github.com/<you>/mucalc.git
cd mucalc
```

If you already cloned without `--recurse-submodules`:
```
git submodule update --init --recursive
```

## Build

### Windows (PowerShell)
```
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

### macOS
```
cmake -B build -G Xcode
cmake --build build --config Release
```

### Linux
```
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

`COPY_PLUGIN_AFTER_BUILD TRUE` in CMakeLists.txt drops the binaries straight into the OS user plugin folders:

| OS | VST3 | CLAP | AU |
|---|---|---|---|
| Windows | `%LOCALAPPDATA%\Programs\Common\VST3` | `%LOCALAPPDATA%\Programs\Common\CLAP` | — |
| macOS | `~/Library/Audio/Plug-Ins/VST3` | `~/Library/Audio/Plug-Ins/CLAP` | `~/Library/Audio/Plug-Ins/Components` |
| Linux | `~/.vst3` | `~/.clap` | — |

If the auto-copy fails (permissions), build artifacts are under `build/MuCalc_artefacts/Release/`.

## Testing in Bitwig

1. Bitwig → **Settings → Locations → Plug-in Locations** — confirm the user VST3 and CLAP folders above are listed.
2. **Settings → Plug-ins** → click the rescan icon next to each path.
3. Find **MuCalc** under **Audio FX**.
4. Drop it on any track. Change the project tempo and the table updates within ~50 ms. Untick "Sync to host" and edit the BPM in the readout — your manual value sticks regardless of project tempo.

## Project layout

```
CMakeLists.txt                      top-level build
JUCE/                               git submodule (pinned)
clap-juce-extensions/               git submodule
Source/
  PluginProcessor.h / .cpp          AudioProcessor + 3 APVTS parameters + host-tempo read
  PluginEditor.h / .cpp             custom UI: BPM control, sync toggle, mode selector, table
  DivisionTableComponent.h / .cpp   7-row × 3-column table; click cell → copy
  NoteDivisions.h                   pure data + math (header-only)
.github/workflows/build.yml         cross-platform CI matrix
```

## License notes

JUCE 8 is dual-licensed. Personal/educational use (under the JUCE revenue threshold) is free, but the splash screen is mandatory unless you have a paid license. macOS distribution outside your own machine requires Apple Developer signing + notarization.
