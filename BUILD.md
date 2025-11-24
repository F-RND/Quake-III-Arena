# Building Quake III Arena - Modernized

## Quick Start

### Linux

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libx11-dev \
    libxxf86dga-dev \
    libxxf86vm-dev \
    libxext-dev

# Configure and build
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Build Options

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \          # Release or Debug
    -DBUILD_CLIENT=ON \                   # Build main client
    -DBUILD_SERVER=OFF \                  # Build dedicated server (currently disabled)
    -DUSE_INTERNAL_JPEG=ON \             # Use internal JPEG library
    -DUSE_OPENAL=ON \                    # Enable OpenAL audio (if available)
    -DUSE_CURL=ON                        # Enable libcurl downloads (if available)
```

## Current Build Status

⚠️ **The build is not yet complete** - There are remaining linker errors that need to be resolved:

### Known Issues

1. **Botlib inline functions** - Missing external definitions
   - `AAS_TravelFlagForType_inline`
   - `AAS_AreaContentsTravelFlags_inline`
   - `AAS_ClusterAreaNum`
   - `AAS_RoutingTime`

2. **Missing allocation functions**
   - `Com_Allocate` (used by splines)
   - `Com_Dealloc` (used by splines)

3. **Dedicated server temporarily disabled**
   - Additional undefined references
   - Will be re-enabled once client build is stable

## Compiler Requirements

- **GCC** 7.0+ or **Clang** 6.0+
- **CMake** 3.15+
- **C11** and **C++17** support

## Platform Support

### Linux (Primary)
- ✅ x86_64 architecture
- ✅ x86 (32-bit) architecture
- Build system fully tested on Ubuntu 24.04

### Windows
- ⏳ Visual Studio 2019+ (untested)
- ⏳ MinGW-w64 (untested)

### macOS
- ⏳ Xcode 11+ (untested)
- ⏳ Apple Silicon support planned

## Build Outputs

### Client
- **Binary:** `build/bin/quake3`
- **Game Modules:**
  - `build/baseq3/qagamex86_64.so` - Server-side game logic
  - `build/baseq3/cgamex86_64.so` - Client-side game/rendering
  - `build/baseq3/uix86_64.so` - User interface

### Dedicated Server (when re-enabled)
- **Binary:** `build/bin/quake3ded`

## Troubleshooting

### Missing OpenGL

```bash
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev
```

### Missing X11

```bash
sudo apt-get install libx11-dev libxxf86dga-dev libxxf86vm-dev libxext-dev
```

### Architecture Detection Issues

The build system auto-detects your architecture. If you encounter issues:

```bash
# Force architecture
cmake .. -DCMAKE_SYSTEM_PROCESSOR=x86_64
```

## Development Build

For development with debug symbols and no optimization:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

## Clean Build

```bash
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Build Warnings

Many warnings are expected from the original 2005 codebase:
- Type sign mismatches
- Pointer-to-int casts (being addressed for 64-bit)
- Deprecated OpenGL functions (will be addressed in renderer modernization)

## Next Steps

Once the build is complete and stable:
1. Runtime testing with original Q3A data files
2. Memory leak detection (valgrind)
3. Performance profiling
4. Begin architectural modernization (see MODERNIZATION.md)

## Contributing

This is an active modernization effort. Current focus:
1. Fixing remaining linker errors
2. Completing first successful build
3. Basic runtime stability

See `MODERNIZATION.md` for the full project roadmap.

---

**Build System Version:** CMake 3.15+
**Last Updated:** 2025-11-23
