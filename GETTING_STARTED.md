# Getting Started - Modernized Quake III Arena

Complete guide to building, running, testing, and modding your modern Q3A build with advanced movement, procedural arenas, and RTX Remix support.

---

## Table of Contents

1. [Building Locally](#1-building-locally)
2. [Getting Game Data Files](#2-getting-game-data-files)
3. [Running the Game](#3-running-the-game)
4. [Testing Movement Systems](#4-testing-movement-systems)
5. [Adding Custom Assets](#5-adding-custom-assets)
6. [Map Creation](#6-map-creation)
7. [RTX Remix Setup](#7-rtx-remix-setup)
8. [Troubleshooting](#8-troubleshooting)

---

## 1. Building Locally

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libx11-dev \
    libxxf86dga-dev \
    libxxf86vm-dev \
    libxext-dev

# Fedora/RHEL
sudo dnf install -y \
    gcc gcc-c++ \
    cmake \
    git \
    mesa-libGL-devel \
    mesa-libGLU-devel \
    libX11-devel \
    libXxf86dga-devel \
    libXxf86vm-devel \
    libXext-devel
```

### Build Steps

```bash
# Clone the repository (if you haven't already)
git clone <your-repo-url>
cd Quake-III-Arena

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build (use -j for parallel compilation)
make -j$(nproc)

# Or use cmake to build
cmake --build . -j$(nproc)
```

### Build Output

After successful build, you'll have:

```
build/
├── quake3              # Main client executable
├── quake3ded           # Dedicated server
└── baseq3/
    ├── qagamex86_64.so # Server-side game module (advanced movement!)
    ├── cgamex86_64.so  # Client-side game module
    └── uix86_64.so     # UI module
```

---

## 2. Getting Game Data Files

Quake III Arena requires the original game data files (pak files). You need a **legitimate copy** of Q3A.

### Option 1: Steam (Recommended)

1. Purchase/download Q3A from Steam
2. Locate installation: `~/.steam/steam/steamapps/common/Quake 3 Arena/baseq3/`
3. Copy pak files:

```bash
# Copy game data to your build
cp ~/.steam/steam/steamapps/common/Quake\ 3\ Arena/baseq3/pak*.pk3 \
   ~/Quake-III-Arena/build/baseq3/
```

### Option 2: Physical Copy

1. Insert Q3A CD/DVD
2. Copy all `pak*.pk3` files from the CD to `build/baseq3/`

### Required Files

Minimum files needed in `build/baseq3/`:

```
baseq3/
├── pak0.pk3        # Core game data (REQUIRED - ~500MB)
├── pak1.pk3        # Patch 1.27
├── pak2.pk3        # Patch 1.28
├── pak3.pk3        # Patch 1.29
├── pak4.pk3        # Patch 1.30
├── pak5.pk3        # Patch 1.31
├── pak6.pk3        # Patch 1.32
├── pak7.pk3        # Team Arena (optional)
├── pak8.pk3        # Team Arena (optional)
└── qagamex86_64.so # Your custom build
```

---

## 3. Running the Game

### Basic Launch

```bash
cd build
./quake3
```

### Launch with Console

```bash
./quake3 +set r_fullscreen 0 +set r_mode 8 +set com_zoneMegs 64
```

Common launch parameters:

```bash
# Windowed mode, 1920x1080, 128MB zone memory
./quake3 \
  +set r_fullscreen 0 \
  +set r_customwidth 1920 \
  +set r_customheight 1080 \
  +set r_mode -1 \
  +set com_zoneMegs 128 \
  +set sv_pure 0

# Enable developer mode
./quake3 +set developer 1 +set timescale 1

# Start a listen server directly
./quake3 +map q3dm17 +set g_gametype 0
```

### Dedicated Server

```bash
./quake3ded \
  +set dedicated 2 \
  +set net_port 27960 \
  +exec server.cfg
```

Create `baseq3/server.cfg`:

```
seta sv_hostname "My Modern Q3A Server"
seta g_gametype 0        // 0=FFA, 1=Tournament, 3=Team DM, 4=CTF
seta sv_maxclients 16
seta g_motd "Advanced Movement Enabled!"
set rconpassword "yourpassword"
map q3dm17
```

---

## 4. Testing Movement Systems

### Setting Up Keybinds

Open the console (press **~** or **`**) and enter:

```
// Advanced Movement Keybinds
bind z "+button6"        // Bullet time modifier (BUTTON_MODIFIER1)
bind c "+button7"        // Wall run modifier (BUTTON_MODIFIER2)
bind ctrl "+button8"     // Slide modifier (BUTTON_MODIFIER3)
bind shift "+button9"    // Dodge/roll (BUTTON_DODGE)

// Or create an autoexec.cfg file
writeconfig movement
```

Create `baseq3/movement.cfg`:

```
// Advanced Movement Configuration
// Paste these into console or exec movement.cfg

// Modifier keys
bind z "+button6"        // Bullet time
bind c "+button7"        // Wall run
bind ctrl "+button8"     // Slide
bind shift "+button9"    // Dodge

// Movement display
cg_drawFPS 1
cg_drawSpeed 1           // If available
developer 1              // Show debug info

// Helpful testing commands
bind f1 "god"            // Toggle god mode
bind f2 "noclip"         // Toggle noclip
bind f3 "timescale 0.5"  // Slow motion test
bind f4 "timescale 1.0"  // Normal time

echo "^2Advanced Movement Config Loaded!"
```

### Testing Each Mechanic

#### 1. **Double Jump**
```
// In-game test
1. Start a map: /map q3dm17
2. Jump (Space)
3. Release Space
4. Press Space again in mid-air
5. You should jump again at 80% height!
```

#### 2. **Dodge/Roll**
```
// Test all 8 directions
1. Hold W (forward)
2. Press Shift
3. You should roll forward quickly
4. Try: W+Shift, S+Shift, A+Shift, D+Shift, W+D+Shift, etc.
```

#### 3. **Wall Running**
```
// Find a wall (q3dm17 has many)
1. Run toward a vertical wall
2. When close, press: C + Space + Shift
3. You should run along the wall!
4. Press Space to jump off with boost
```

#### 4. **Sliding**
```
// Need speed first
1. Sprint forward (W)
2. When at full speed, press: Ctrl + Space + Shift
3. You should slide with speed boost!
4. You can shoot while sliding
```

#### 5. **Bullet Time**
```
// Slow motion combat
1. Hold Z
2. Press Attack (Mouse1)
3. Time should slow to 30% for everyone
4. You maintain full speed
5. Watch energy drain (top right if HUD is enabled)
```

### Testing Console Commands

```
// Enable cheats
/devmap q3dm17

// God mode for testing
/god

// Noclip for wall run testing
/noclip

// Check movement state
/cg_thirdPerson 1       // See your character
/cg_thirdPersonRange 100

// Test procedural arenas
/generate_arena 12345 1 0   // seed=12345, depth=1, theme=Tech
/start_roguelike 12345      // Start roguelike run
/advance_arena              // Next depth
/end_roguelike              // End run

// Timescale testing
/timescale 0.3              // Manually slow time
/timescale 1.0              // Normal speed
/timescale 2.0              // Fast forward

// Give all weapons
/give all
```

### Movement Debug Display

Add to `baseq3/autoexec.cfg`:

```
// Developer HUD
seta developer 1
seta cg_drawFPS 1
seta com_speeds 1

// Third person for movement visualization
bind f5 "toggle cg_thirdPerson; toggle cg_thirdPersonRange 100 200"

// Quick load configs
bind f9 "exec movement.cfg"
bind f10 "exec server.cfg"
```

---

## 5. Adding Custom Assets

Q3A uses **PK3 files** (renamed ZIP archives) to package assets.

### Asset Structure

```
your_custom_mod.pk3/
├── textures/
│   ├── base_wall/
│   │   └── mywall.tga (or .jpg)
│   └── models/players/
│       └── myskin/
│           └── skin.tga
├── models/
│   ├── players/
│   │   └── mycharacter/
│   │       ├── head.md3
│   │       ├── upper.md3
│   │       ├── lower.md3
│   │       └── animation.cfg
│   └── weapons/
│       └── mygun/
│           └── mygun.md3
├── maps/
│   ├── mymap.bsp
│   └── mymap.aas (bot navigation)
├── sound/
│   └── mysounds/
│       └── footstep.wav
└── scripts/
    ├── myshaders.shader
    └── mymap.arena
```

### Creating a PK3 File

```bash
# Create directory structure
mkdir -p my_mod/textures/custom
mkdir -p my_mod/models/players
mkdir -p my_mod/maps
mkdir -p my_mod/scripts

# Add your assets
cp my_texture.tga my_mod/textures/custom/

# Create PK3 (it's just a ZIP!)
cd my_mod
zip -r ../pak99_mymod.pk3 *

# Install
cp pak99_mymod.pk3 ~/Quake-III-Arena/build/baseq3/
```

### Adding Custom Textures

#### 1. Texture Format

Supported formats:
- **TGA** (Targa) - 24-bit or 32-bit with alpha
- **JPG** - For photos/complex textures
- **PNG** - Supported in some engines (test first)

Recommended:
- **Power of 2 dimensions**: 128x128, 256x256, 512x512, 1024x1024
- **Mipmaps**: Include or let engine generate
- **TGA format** for best compatibility

```bash
# Convert PNG to TGA with ImageMagick
convert mytexture.png -define tga:image-origin=bottom mytexture.tga
```

#### 2. Creating a Shader

Shaders define how textures behave (glow, animation, transparency).

Create `scripts/myshaders.shader`:

```
textures/custom/glowing_wall
{
    qer_editorimage textures/custom/mywall.tga
    surfaceparm nomarks
    {
        map $lightmap
        rgbGen identity
    }
    {
        map textures/custom/mywall.tga
        blendFunc GL_DST_COLOR GL_ZERO
        rgbGen identity
    }
    {
        map textures/custom/mywall_glow.tga
        blendfunc GL_ONE GL_ONE
        rgbGen wave sin 0.5 0.5 0 1
    }
}

textures/custom/animated_water
{
    qer_editorimage textures/custom/water1.tga
    surfaceparm trans
    surfaceparm nonsolid
    surfaceparm water
    {
        animmap 2 textures/custom/water1.tga textures/custom/water2.tga textures/custom/water3.tga
        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        rgbGen identity
    }
}
```

### Adding Custom Player Models

Q3A uses **MD3 format** for models.

#### Converting Blender Models to MD3

```bash
# Install Blender MD3 exporter
# https://github.com/axredneck/blender-md3

# In Blender:
# 1. Model your character (keep under 1000 polys per mesh)
# 2. UV unwrap
# 3. Export as MD3
# 4. Create: head.md3, upper.md3, lower.md3
```

#### Player Model Structure

```
models/players/mycharacter/
├── head.md3              # Head mesh
├── upper.md3             # Torso mesh
├── lower.md3             # Legs mesh
├── animation.cfg         # Animation definitions
├── icon_default.tga      # Selection icon
└── skin_default.skin     # Texture mapping

# Texture files
textures/models/players/mycharacter/
├── head.tga
├── upper.tga
└── lower.tga
```

#### animation.cfg Example

```
sex m                     // m=male, f=female, n=neutral
footsteps default         // Footstep sound type
headoffset 0 0 0         // Head attachment point

// Format: first_frame num_frames looping_frames fps
// LEGS animations
0   40  40  25          // LEGS_WALKCR
40  12  12  25          // LEGS_WALK
52  20  20  25          // LEGS_RUN
72  6   6   30          // LEGS_BACK
78  8   8   25          // LEGS_SWIM
86  12  -1  25          // LEGS_JUMP (NEW: For double jump)
98  18  -1  25          // LEGS_LAND
116 6   6   15          // LEGS_JUMPB
122 10  -1  25          // LEGS_WALLJUMP (NEW: For wall running)
132 12  -1  30          // LEGS_SLIDE (NEW: For sliding)
144 8   -1  25          // LEGS_DODGE (NEW: For dodge/roll)

// TORSO animations
152 12  0   25          // TORSO_GESTURE
164 12  12  25          // TORSO_ATTACK
...
```

### Adding Custom Weapons

```
models/weapons2/mygun/
├── mygun.md3             # Main model
├── mygun_hand.md3        # First-person view
├── weapon.cfg            // Weapon properties
└── skin_default.skin     // Texture mapping

textures/models/weapons2/mygun/
└── mygun.tga
```

---

## 6. Map Creation

### Using GtkRadiant (Q3A Level Editor)

#### Installation

```bash
# Download GtkRadiant 1.6 or NetRadiant
# https://icculus.org/gtkradiant/
# or https://netradiant.gitlab.io/

# Ubuntu
sudo apt-get install netradiant netradiant-gamepack-q3data

# Run
netradiant
```

#### Quick Map Tutorial

1. **File → New Map**
2. Select "Quake 3 Arena" game type
3. Create brushes (solid geometry)
4. Add entities:
   - `info_player_deathmatch` - Player spawn
   - `weapon_rocketlauncher` - Weapon pickup
   - `light` - Light source
5. **Build → BSP**
6. **Build → -vis**
7. **Build → -light -fast**

#### Testing Your Map

```bash
# BSP file goes in baseq3/maps/
cp mymap.bsp ~/Quake-III-Arena/build/baseq3/maps/

# In game console
/devmap mymap
```

#### Arena File for Map Selection

Create `scripts/mymap.arena`:

```
{
    map "mymap"
    longname "My Custom Arena"
    fraglimit 20
    timelimit 15
    type "ffa tourney team ctf"
    bots "sarge slash"
}
```

### Procedural Arena Testing

```bash
# Start Q3A
./quake3

# In console
/devmap q3dm17

# Generate procedural arena
/generate_arena 42 5 1    # seed=42, depth=5, theme=Gothic

# Start roguelike run
/start_roguelike 12345

# Progress through depths
/advance_arena
/advance_arena
/advance_arena

# End run
/end_roguelike
```

---

## 7. RTX Remix Setup

### Prerequisites

- **NVIDIA RTX GPU** (20/30/40 series)
- **Windows** (RTX Remix currently Windows-only)
- **Vulkan-compatible** Q3A build or wrapper

### Option 1: Using DXVK (Recommended)

```bash
# DXVK translates D3D9 to Vulkan, which Remix can capture

# 1. Download DXVK
# https://github.com/doitsujin/dxvk/releases

# 2. Install DXVK in Q3A directory
cp dxvk/x64/d3d9.dll ./

# 3. Download RTX Remix
# https://www.nvidia.com/en-us/geforce/rtx-remix/

# 4. Install RTX Remix Runtime
cp NvRemixBridge.dll ./
cp .trex directory ./
```

### Option 2: Vulkan Port

Consider porting Q3A renderer to Vulkan:
- **vkQuake3**: https://github.com/suijingfeng/vkQuake3
- **Quake3e Vulkan**: https://github.com/ec-/Quake3e

### Preparing Assets for RTX Remix

#### Texture Best Practices

```
Original Q3A Resolution: 256x256 - 512x512
RTX Remix Target: 1024x1024 - 4096x4096

1. Upscale textures using AI:
   - Topaz Gigapixel AI
   - ESRGAN
   - Waifu2x

2. Create PBR textures:
   - Albedo/Diffuse
   - Normal map
   - Metallic map
   - Roughness map
   - Emissive map (for glowing surfaces)

3. Use PNG or TGA with alpha
```

#### RTX Remix Workflow

```
1. Launch Q3A with RTX Remix Runtime
2. Press Alt+X to open RTX Remix
3. Capture scene
4. Replace textures with high-res PBR versions
5. Add ray-traced lighting
6. Export remastered assets
7. Test performance
```

### Performance Optimization for RTX

Create `baseq3/rtx_config.cfg`:

```
// Optimize for RTX Remix
seta r_mode -1
seta r_customwidth 1920
seta r_customheight 1080
seta r_fullscreen 1
seta r_swapinterval 0        // Disable vsync
seta r_primitives 2          // Use optimized primitives
seta com_maxfps 144          // Cap framerate

// Reduce non-RT effects
seta cg_shadows 0            // RT handles shadows
seta cg_marks 0              // RT handles decals
seta r_dynamiclight 0        // RT handles lighting
```

### Creating RTX-Ready Custom Maps

```
Map Design Tips for RTX:
1. Add more light sources (RTX loves lights!)
2. Use emissive textures for glowing surfaces
3. Design reflective surfaces (water, metal)
4. Add transparent materials (glass, plasma)
5. Include outdoor areas for global illumination
6. Use high-poly models (RTX can handle it)
```

---

## 8. Troubleshooting

### Build Issues

**Error: Missing OpenGL libraries**
```bash
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev
```

**Error: Cannot find pak0.pk3**
```bash
# Verify file location
ls -lh build/baseq3/pak*.pk3

# Should show pak0.pk3 (~500MB)
```

**Error: Architecture mismatch**
```bash
# Game modules must match architecture
# Check with:
file build/baseq3/*.so

# Should show: ELF 64-bit
```

### Runtime Issues

**Game crashes on startup**
```bash
# Run with debug output
./quake3 +set developer 1 +set logfile 1

# Check logfile
cat ~/.q3a/baseq3/qconsole.log
```

**Movement not working**
```bash
# Verify keybinds
bind z "+button6"
bind c "+button7"
bind ctrl "+button8"
bind shift "+button9"

# Check in console
/cvarlist button

# Test individual moves
# Should show button states
```

**Procedural arenas don't generate**
```bash
# Verify game module loaded
/which qagame

# Should show: baseq3/qagamex86_64.so

# Try with devmap
/devmap q3dm17
/generate_arena 1 1 0
```

### Custom Asset Issues

**Textures not loading**
```bash
# Check file paths (case-sensitive!)
# Correct:   textures/custom/mywall.tga
# Incorrect: Textures/Custom/MyWall.tga

# Verify in PK3
unzip -l pak99_mymod.pk3
```

**Model not appearing**
```bash
# Check MD3 format
# Must be valid MD3 version 15

# Verify skin file
cat models/players/mychar/skin_default.skin

# Should have:
# head,textures/models/players/mychar/head.tga
# upper,textures/models/players/mychar/upper.tga
# lower,textures/models/players/mychar/lower.tga
```

**Map crashes on load**
```bash
# Recompile BSP
# In GtkRadiant: Build → Clean, then rebuild all

# Check for leaks
# Build → -vis should complete without errors

# Verify entities
# Must have at least one info_player_deathmatch
```

### Performance Issues

```bash
# Lower graphics settings
/r_picmip 1              // Lower texture quality
/r_ext_compressed_textures 1
/r_ext_multitexture 1
/r_subdivisions 20       // Curve quality

# Increase performance
/com_maxfps 125
/cg_shadows 0
/cg_brass 0
/cg_gibs 0
```

---

## Quick Start Checklist

- [ ] Built project with `cmake --build build`
- [ ] Copied `pak0.pk3` to `build/baseq3/`
- [ ] Launched game with `./quake3`
- [ ] Created `movement.cfg` with keybinds
- [ ] Tested double jump on `q3dm17`
- [ ] Tested dodge/roll
- [ ] Tested wall running
- [ ] Tested slide
- [ ] Tested bullet time
- [ ] Generated procedural arena with `/generate_arena`
- [ ] Created custom PK3 file
- [ ] Added custom texture
- [ ] Created test map in Radiant
- [ ] Configured for RTX Remix (optional)

---

## Useful Resources

### Official Tools
- **GtkRadiant**: https://icculus.org/gtkradiant/
- **NetRadiant**: https://netradiant.gitlab.io/
- **Blender MD3 Addon**: https://github.com/axredneck/blender-md3

### Community Resources
- **Q3A Mapping**: http://www.3dattack.ca/Quake-3/
- **Shader Manual**: https://q3map2.github.io/docs/shader_manual/
- **Model Tutorial**: http://jkhub.org/tutorials/article/46-making-a-character-for-quake-3/

### RTX Remix
- **Official Site**: https://www.nvidia.com/en-us/geforce/rtx-remix/
- **Documentation**: https://github.com/NVIDIAGameWorks/rtx-remix

### Modern Q3A Engines
- **ioquake3**: https://ioquake3.org/
- **Quake3e**: https://github.com/ec-/Quake3e
- **vkQuake3**: https://github.com/suijingfeng/vkQuake3

---

**Happy Modding! 🚀**

Your modernized Q3A build features:
- ✅ Advanced movement (double jump, wall run, dodge, slide, bullet time)
- ✅ Procedural arena generation
- ✅ Modern build system (CMake, GCC 13+, 64-bit)
- ✅ Ready for custom assets and RTX Remix

Next steps: Test everything, create awesome maps, and share your creations!
