# RTX Remix Guide - Post-Processing Pipeline

Comprehensive guide for using NVIDIA RTX Remix with your modernized Quake III Arena build to create stunning ray-traced visuals.

---

## Table of Contents

1. [RTX Remix Overview](#1-rtx-remix-overview)
2. [System Requirements](#2-system-requirements)
3. [Setting Up Q3A for RTX Remix](#3-setting-up-q3a-for-rtx-remix)
4. [Asset Preparation Pipeline](#4-asset-preparation-pipeline)
5. [Texture Remastering](#5-texture-remastering)
6. [Model Enhancement](#6-model-enhancement)
7. [Lighting and Materials](#7-lighting-and-materials)
8. [Performance Optimization](#8-performance-optimization)
9. [Complete Workflow Example](#9-complete-workflow-example)

---

## 1. RTX Remix Overview

### What is RTX Remix?

RTX Remix is NVIDIA's tool for remastering classic DirectX 8/9 games with:
- **Path-traced lighting** (full ray tracing)
- **Physically Based Rendering (PBR)**
- **AI-upscaled textures**
- **Enhanced materials** (metallic, roughness, emissive)
- **DLSS 3** support

### How It Works

```
Original Game
    ↓
RTX Remix Runtime (captures frames)
    ↓
RTX Remix Toolkit (edit scene)
    ↓
Replace assets with high-quality versions
    ↓
Remastered Game with Path Tracing
```

---

## 2. System Requirements

### Minimum

- **GPU**: NVIDIA RTX 2060 or higher
- **VRAM**: 6GB minimum, 8GB+ recommended
- **RAM**: 16GB system memory
- **OS**: Windows 10/11 (64-bit)
- **DirectX**: 11 or 12
- **Drivers**: Latest NVIDIA Game Ready drivers

### Recommended

- **GPU**: RTX 3070 or higher
- **VRAM**: 12GB+
- **RAM**: 32GB
- **CPU**: 8+ cores
- **Storage**: SSD with 50GB+ free space

### Software

```
Required:
- NVIDIA RTX Remix Runtime
- NVIDIA RTX Remix Toolkit
- Q3A with D3D9 renderer or DXVK wrapper

Optional:
- Adobe Substance 3D Designer/Painter
- Blender 3.0+
- GIMP/Photoshop
- AI Upscaling: Topaz Gigapixel, ESRGAN
```

---

## 3. Setting Up Q3A for RTX Remix

### Method 1: DXVK Wrapper (Recommended)

DXVK translates Direct3D 9 to Vulkan, which RTX Remix can capture.

```bash
# 1. Download DXVK
# https://github.com/doitsujin/dxvk/releases

# 2. Extract and copy files to Q3A directory
# For 64-bit Q3A:
cp dxvk-2.3/x64/d3d9.dll /path/to/quake3/
cp dxvk-2.3/x64/dxgi.dll /path/to/quake3/

# 3. Download RTX Remix Runtime
# https://www.nvidia.com/en-us/geforce/rtx-remix/

# 4. Install RTX Remix files
cp NvRemixBridge.dll /path/to/quake3/
cp -r .trex /path/to/quake3/
```

### Directory Structure

```
Quake-III-Arena/
├── quake3.exe              # Your build
├── d3d9.dll                # DXVK wrapper
├── dxgi.dll                # DXVK component
├── NvRemixBridge.dll       # RTX Remix runtime
├── .trex/                  # RTX Remix config
│   ├── config.toml
│   └── captures/
└── baseq3/
    ├── pak0.pk3
    └── qagamex86_64.so
```

### RTX Remix Configuration

Create `.trex/config.toml`:

```toml
[remix]
runtime.enabled = true
runtime.captureKey = "F10"
runtime.menuKey = "Alt+X"

[rendering]
pathtracing.enabled = true
pathtracing.bounces = 4
pathtracing.samplesPerPixel = 2

dlss.enabled = true
dlss.quality = "Balanced"  # Auto, Performance, Balanced, Quality, UltraQuality

[enhancement]
textureReplacementEnabled = true
materialReplacementEnabled = true
lightReplacementEnabled = true

[capture]
outputDirectory = ".trex/captures"
autoCapture = false
```

### Launch Configuration

Create `launch_rtx.bat` (Windows):

```batch
@echo off
REM Set environment for RTX Remix
set DXVK_HUD=fps,compiler
set DXVK_LOG_LEVEL=none

REM Launch Q3A with RTX
quake3.exe +set r_mode -1 ^
           +set r_customwidth 1920 ^
           +set r_customheight 1080 ^
           +set r_fullscreen 1 ^
           +set com_maxfps 144

pause
```

---

## 4. Asset Preparation Pipeline

### Overall Workflow

```
1. Extract Original Assets
    ↓
2. Upscale Textures (AI)
    ↓
3. Create PBR Maps (Normal, Metallic, Roughness)
    ↓
4. Import to RTX Remix
    ↓
5. Fine-tune Materials
    ↓
6. Export Remastered Assets
    ↓
7. Package as Mod
```

### Extracting Q3A Assets

```bash
# Q3A PK3 files are ZIP archives
mkdir extracted_assets
cd extracted_assets

# Extract all textures
unzip ../baseq3/pak0.pk3 "textures/*"

# Extract models
unzip ../baseq3/pak0.pk3 "models/*"

# Extract maps
unzip ../baseq3/pak0.pk3 "maps/*"
```

### Asset Organization

```
rtx_remake/
├── source/                     # Original extracted assets
│   ├── textures/
│   ├── models/
│   └── maps/
├── upscaled/                   # AI upscaled versions
│   └── textures/
├── pbr/                        # PBR material maps
│   ├── albedo/
│   ├── normal/
│   ├── metallic/
│   ├── roughness/
│   └── emissive/
└── final/                      # RTX Remix ready assets
    └── textures/
```

---

## 5. Texture Remastering

### Step 1: AI Upscaling

#### Using ESRGAN (Free, Open Source)

```bash
# Install Real-ESRGAN
pip install realesrgan

# Upscale 4x
realesrgan-ncnn-vulkan -i input.tga -o output.png -s 4

# Batch process
for file in source/textures/**/*.tga; do
    realesrgan-ncnn-vulkan -i "$file" -o "upscaled/${file%.tga}.png" -s 4
done
```

#### Using Topaz Gigapixel AI (Commercial)

```
1. Open Topaz Gigapixel AI
2. Load Q3A texture
3. Set upscale factor: 4x (256×256 → 1024×1024)
4. Model: Standard v1 or CG Art
5. Reduce noise: 0-20%
6. Enhance details: 30-50%
7. Export as PNG/TGA
```

### Step 2: Creating PBR Materials

#### Automatic PBR Generation

**Using Substance 3D Designer:**

```
1. Import upscaled albedo texture
2. Bitmap to Material (Image to Material workflow)
3. Adjust parameters:
   - Normal intensity: 1.0-3.0
   - Roughness variation: 0.2-0.8
   - Metallic areas: mask selection
4. Export texture set:
   - BaseColor (Albedo)
   - Normal
   - Metallic
   - Roughness
   - AO (Ambient Occlusion)
   - Emissive (if needed)
```

#### Manual PBR Creation

**Normal Map (from Albedo):**

```python
# Using PIL and numpy
from PIL import Image
import numpy as np

def generate_normal_map(albedo_path, output_path, strength=1.0):
    img = Image.open(albedo_path).convert('L')  # Grayscale
    data = np.array(img, dtype=float)

    # Sobel filter for gradients
    dx = np.gradient(data, axis=1)
    dy = np.gradient(data, axis=0)

    # Create normal map
    normal = np.zeros((data.shape[0], data.shape[1], 3))
    normal[:,:,0] = dx * strength          # R = X
    normal[:,:,1] = -dy * strength         # G = Y  (flip for OpenGL)
    normal[:,:,2] = 1.0                    # B = Z

    # Normalize
    length = np.sqrt(normal[:,:,0]**2 + normal[:,:,1]**2 + normal[:,:,2]**2)
    normal = normal / length[:,:,np.newaxis]

    # Convert to 0-255 range
    normal = ((normal + 1.0) * 127.5).astype(np.uint8)

    Image.fromarray(normal).save(output_path)

# Usage
generate_normal_map('upscaled/wall.png', 'pbr/normal/wall_n.png', strength=2.0)
```

**Roughness Map:**

```python
def generate_roughness_map(albedo_path, output_path, base_roughness=0.5):
    img = Image.open(albedo_path).convert('L')
    data = np.array(img, dtype=float) / 255.0

    # Darker = rougher, lighter = smoother
    # Invert for metallic surfaces
    roughness = 1.0 - (data * 0.3)  # Adjust multiplier
    roughness = np.clip(roughness, 0.1, 0.9)

    roughness_img = (roughness * 255).astype(np.uint8)
    Image.fromarray(roughness_img).save(output_path)
```

**Metallic Map:**

```python
def generate_metallic_map(albedo_path, output_path, threshold=200):
    img = Image.open(albedo_path).convert('RGB')
    data = np.array(img)

    # Bright pixels = metallic (e.g., white = metal)
    brightness = np.mean(data, axis=2)
    metallic = (brightness > threshold).astype(np.uint8) * 255

    Image.fromarray(metallic).save(output_path)
```

### Step 3: Texture Naming Convention

RTX Remix expects specific naming:

```
original_texture.tga → Upscaled versions:

base_wall_metal.tga     (original)
    ↓
base_wall_metal_Albedo.png        # Diffuse color
base_wall_metal_Normal.png        # Normal/bump map
base_wall_metal_Metallic.png      # Metallic mask (0=dielectric, 1=metal)
base_wall_metal_Roughness.png     # Roughness (0=mirror, 1=rough)
base_wall_metal_Emissive.png      # Self-illumination (optional)
base_wall_metal_AO.png            # Ambient occlusion (optional)
```

---

## 6. Model Enhancement

### High-Poly Model Replacement

```
Original Q3A model: ~800 polygons
RTX Remix target: 5,000-50,000 polygons
```

#### Using Blender

```python
# Blender script for enhancing Q3A models

import bpy

# Import MD3
bpy.ops.import_scene.md3(filepath="models/players/sarge/upper.md3")

# Subdivide mesh
obj = bpy.context.active_object
bpy.ops.object.mode_set(mode='EDIT')
bpy.ops.mesh.subdivide(number_cuts=2)  # Increase poly count

# Smooth shading
bpy.ops.object.mode_set(mode='OBJECT')
bpy.ops.object.shade_smooth()

# Add details (normal mapping alternative)
modifier = obj.modifiers.new(name="Subdivision", type='SUBSURF')
modifier.levels = 2

# Export as enhanced MD3 or FBX for RTX Remix
bpy.ops.export_scene.fbx(filepath="enhanced/sarge_upper.fbx")
```

### Model Texture Atlas

Create high-resolution UV maps:

```
Original: 256×256 skin texture
Enhanced: 2048×2048 or 4096×4096

Steps:
1. UV unwrap in Blender
2. Bake PBR textures
3. Export with proper UVs
4. Replace in RTX Remix
```

---

## 7. Lighting and Materials

### Adding Emissive Materials

```glsl
// Shader example for glowing surfaces
Emissive Map Intensity: 2.0-10.0

// Q3A shader format (for reference)
textures/lights/glowlight
{
    qer_editorimage textures/lights/glowlight.tga
    surfaceparm nomarks
    q3map_surfacelight 1000    // Q3 light emission
    {
        map textures/lights/glowlight.tga
        blendFunc GL_ONE GL_ONE
        rgbGen wave sin 0.5 0.5 0 1
    }
}

// RTX Remix: Use emissive map with high intensity
// File: glowlight_Emissive.png (bright white/color)
```

### RTX Remix Material Properties

```json
{
  "material_name": "base_wall_metal",
  "albedo": "base_wall_metal_Albedo.png",
  "normal": "base_wall_metal_Normal.png",
  "metallic": "base_wall_metal_Metallic.png",
  "roughness": "base_wall_metal_Roughness.png",
  "emissive": "base_wall_metal_Emissive.png",

  "properties": {
    "roughness_factor": 0.5,
    "metallic_factor": 0.8,
    "emissive_intensity": 0.0,
    "normal_strength": 1.0,
    "opacity": 1.0
  }
}
```

### Light Placement

RTX Remix best practices:

```
1. Convert Q3 light entities to RTX lights
2. Add area lights (not point lights)
3. Use emissive materials for screens/panels
4. Outdoor: Add sun/sky lighting
5. Indoor: Add bounce lights
```

---

## 8. Performance Optimization

### Resolution Ladder

```
Texture Resolution Strategy:
- Close-up surfaces (walls, floors): 2048×2048
- Mid-distance (ceilings, props): 1024×1024
- Far distance (skybox, details): 512×512
- UI elements: 4096×4096 (clarity)
```

### Geometry Optimization

```
Model Polygon Budget:
- Player models: 10,000-15,000 tris
- Weapons: 5,000-8,000 tris
- Props: 1,000-3,000 tris
- Environment: Use instances for repeated geometry
```

### RTX Settings for Q3A

```toml
# .trex/config.toml - Performance balance

[pathtracing]
bounces = 3                    # Lower for performance (3-6)
samplesPerPixel = 1            # RTX 4090: 2-4, RTX 3070: 1-2
russianRoulette = true

[denoising]
enabled = true
quality = "Balanced"           # Fast, Balanced, Quality

[dlss]
enabled = true
quality = "Balanced"           # Massive FPS boost
sharpness = 0.5

[performance]
asyncCompute = true
textureStreaming = true
lodBias = 0                    # 0=full quality, 1=lower quality at distance
```

### Optimization Checklist

- [ ] Reduce texture resolution for distant objects
- [ ] Enable DLSS (2-3x FPS increase)
- [ ] Lower path tracing bounces (3 instead of 8)
- [ ] Use texture streaming
- [ ] Reduce samples per pixel (1-2 for real-time)
- [ ] Enable RTX IO (faster texture loading)
- [ ] Optimize shaders (remove unused channels)

---

## 9. Complete Workflow Example

### Example: Remastering q3dm17 (The Longest Yard)

#### Step 1: Extract Assets

```bash
unzip baseq3/pak0.pk3 "maps/q3dm17.bsp"
unzip baseq3/pak0.pk3 "textures/base_wall/*"
unzip baseq3/pak0.pk3 "textures/sfx/*"
```

#### Step 2: Upscale Textures

```bash
# Process all base_wall textures
for texture in textures/base_wall/*.tga; do
    realesrgan-ncnn-vulkan -i "$texture" \
                           -o "upscaled/$(basename ${texture%.tga}).png" \
                           -s 4
done
```

#### Step 3: Generate PBR Maps

```python
import os
from PIL import Image
import numpy as np

upscaled_dir = "upscaled/"
output_dir = "pbr/"

for filename in os.listdir(upscaled_dir):
    if filename.endswith('.png'):
        base = filename[:-4]
        input_path = os.path.join(upscaled_dir, filename)

        # Generate normal map
        generate_normal_map(
            input_path,
            f"{output_dir}normal/{base}_Normal.png",
            strength=2.0
        )

        # Generate roughness map
        generate_roughness_map(
            input_path,
            f"{output_dir}roughness/{base}_Roughness.png"
        )

        # Generate metallic map
        generate_metallic_map(
            input_path,
            f"{output_dir}metallic/{base}_Metallic.png"
        )

        # Copy albedo
        Image.open(input_path).save(f"{output_dir}albedo/{base}_Albedo.png")
```

#### Step 4: Launch with RTX Remix

```batch
REM launch_rtx.bat
quake3.exe +devmap q3dm17 ^
           +set r_mode -1 ^
           +set r_customwidth 2560 ^
           +set r_customheight 1440 ^
           +set com_maxfps 144
```

#### Step 5: Capture Scene in RTX Remix

```
1. Load map in-game: /devmap q3dm17
2. Walk around to load all textures
3. Press F10 to capture frame
4. Press Alt+X to open RTX Remix Toolkit
```

#### Step 6: Replace Assets in Remix Toolkit

```
In RTX Remix:
1. Click on "base_wall/concrete" texture
2. Replace with "pbr/albedo/concrete_Albedo.png"
3. Add normal map: "pbr/normal/concrete_Normal.png"
4. Add roughness: "pbr/roughness/concrete_Roughness.png"
5. Add metallic: "pbr/metallic/concrete_Metallic.png"
6. Adjust material properties:
   - Roughness: 0.6
   - Metallic: 0.1
   - Normal intensity: 1.2
7. Repeat for all textures
```

#### Step 7: Add Enhanced Lighting

```
In RTX Remix:
1. Add area lights for windows
2. Convert Q3 point lights to sphere lights
3. Add emissive materials for:
   - Jump pads (bright green glow)
   - Teleporters (swirling energy)
   - Health/armor pickups
4. Adjust sun light:
   - Intensity: 10.0
   - Color: Warm white (255, 245, 230)
   - Shadows: Soft, high quality
```

#### Step 8: Export Remastered Mod

```
In RTX Remix:
1. File → Export Assets
2. Choose output directory: "q3dm17_rtx/"
3. Export format: USD (Universal Scene Description)
4. Include:
   ✓ Textures
   ✓ Materials
   ✓ Lights
   ✓ Geometry replacements

5. Package as PK3:
   cd q3dm17_rtx
   zip -r q3dm17_rtx.pk3 *
   cp q3dm17_rtx.pk3 ~/Quake-III-Arena/baseq3/
```

#### Step 9: Test Remastered Map

```bash
./quake3 +devmap q3dm17 +set r_mode -1

# Should show:
# - Path-traced lighting
# - High-res PBR textures
# - Realistic reflections
# - Enhanced materials
# - 60+ FPS with DLSS
```

---

## Before & After Comparison

### Original Q3A
```
Textures: 256×256 (diffuse only)
Polygons: ~50,000 per map
Lighting: Lightmaps (baked)
Shadows: Static only
Materials: Basic diffuse/specular
FPS: 300+ (simple)
```

### RTX Remix Remastered
```
Textures: 2048×2048 (PBR: albedo, normal, metallic, roughness)
Polygons: ~200,000 per map (enhanced geometry)
Lighting: Real-time path tracing (dynamic)
Shadows: Ray-traced soft shadows
Materials: Physically based (metals, glass, emissive)
FPS: 60-120 (with DLSS)
Visual Quality: 10x improvement
```

---

## Helpful Tools & Resources

### AI Upscaling
- **Real-ESRGAN**: https://github.com/xinntao/Real-ESRGAN
- **Topaz Gigapixel AI**: https://www.topazlabs.com/gigapixel-ai
- **Waifu2x**: https://github.com/nagadomi/waifu2x

### PBR Creation
- **Substance Designer**: https://www.adobe.com/products/substance3d-designer.html
- **Materialize**: http://boundingboxsoftware.com/materialize/
- **AwesomeBump**: https://github.com/kmkolasinski/AwesomeBump

### 3D Modeling
- **Blender**: https://www.blender.org/
- **Blender MD3 Addon**: https://github.com/axredneck/blender-md3

### RTX Remix
- **Official**: https://www.nvidia.com/en-us/geforce/rtx-remix/
- **Discord**: https://discord.gg/nvidia
- **Documentation**: https://github.com/NVIDIAGameWorks/rtx-remix

---

## Conclusion

With RTX Remix, your modernized Quake III Arena can achieve:
- **Photorealistic graphics** with path tracing
- **4K/8K textures** via AI upscaling
- **PBR materials** for realistic surfaces
- **Dynamic lighting** that reacts to movement
- **DLSS performance** for high framerates

Combined with the **advanced movement system**, **procedural arenas**, and **custom content**, you have a truly next-gen indie FPS engine built on the legendary Q3A foundation!

**Ready to create the most beautiful arena shooter ever made? Let's go! 🚀**
