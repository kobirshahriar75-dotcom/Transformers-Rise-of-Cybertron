# Transformers: Rise of Cybertron — Master Media Specification & Requirements

This document specifies the complete directory structure, file naming conventions, formats, technical requirements, and licensing considerations for all external media assets.

> **CURRENT AUDIO REQUIREMENT:** The two background-score files listed in Section 3 are required runtime assets. The game does not generate or substitute background music if either file is missing. Existing procedural one-shot sound effects remain separate from the background-score system. Other visual media may still use the documented procedural fallbacks.

---

## 1. Directory Structure

```text
TransformersGame/
│
├── assets/
│   ├── environments/
│   │   ├── level_01_city/
│   │   ├── level_02_industrial/
│   │   ├── level_03_desert/
│   │   ├── level_04_cybertron/
│   │   ├── level_05_frozen/
│   │   ├── level_06_decepticon_base/
│   │   ├── level_07_aerial/
│   │   ├── level_08_metropolis/
│   │   ├── level_09_outskirts/
│   │   └── level_10_final_battlefield/
│   │
│   ├── music/
│   │   ├── opening/
│   │   ├── menu/
│   │   ├── campaign/
│   │   └── boss/
│   │
│   ├── voice/
│   │   ├── optimus/
│   │   ├── megatron/
│   │   └── other_characters/
│   │
│   ├── ui/
│   │   ├── logos/
│   │   ├── icons/
│   │   └── artwork/
│   │
│   └── documentation/
│       └── MEDIA_REQUIREMENTS.md
```

---

## 2. Environments Media Specification (10 Campaign Levels)

For each level, three background layers are supported: `sky.png` (distant background), `midground.png` (spires/structures), and `foreground_props.png` (near parallax detail).

### Level 01: Abandoned City
- **Directory**: `assets/environments/level_01_city/`
- **Files**:
  1. `sky.png` — Background ruins sky; 1920×1080 (16:9); PNG/JPG; Optional; Placeholder: Procedural violet sunset; No special licensing.
  2. `midground.png` — Shattered skyscrapers; 2560×720; 24-bit PNG with alpha; Optional; Placeholder: Procedural dark silhouettes; No special licensing.
  3. `foreground_pylons.png` — Broken highway supports; 1920×720; PNG with alpha; Optional; Placeholder: Procedural hazard pylons; No special licensing.

### Level 02: Industrial District
- **Directory**: `assets/environments/level_02_industrial/`
- **Files**:
  1. `sky.png` — Smog-choked orange/amber industrial skyline; 1920×1080; PNG/JPG; Optional; Placeholder: Procedural amber haze; No special licensing.
  2. `midground.png` — Smelting refineries, chimneys & smoke; 2560×720; PNG with alpha; Optional; Placeholder: Procedural refinery spires; No special licensing.
  3. `foreground_pipes.png` — Glowing molten conduits & exhaust vents; 1920×720; PNG with alpha; Optional; Placeholder: Procedural glowing pipes; No special licensing.

### Level 03: Desert Battlefield
- **Directory**: `assets/environments/level_03_desert/`
- **Files**:
  1. `sky.png` — Harsh metallic Cybertronian desert sky with twin moons; 1920×1080; PNG/JPG; Optional; Placeholder: Procedural bronze sky with moon; No special licensing.
  2. `midground.png` — Iron sand dunes, rusted ship hulls; 2560×720; PNG with alpha; Optional; Placeholder: Procedural layered dunes; No special licensing.

### Level 04: Cybertron Ruins (Boss: Shockwave)
- **Directory**: `assets/environments/level_04_cybertron/`
- **Files**:
  1. `sky.png` — Deep dark indigo Cybertron sky with visible orbital debris; 1920×1080; PNG/JPG; Optional; Placeholder: Procedural cosmic nebula; No special licensing.
  2. `midground.png` — Ancient Cybertronian temples, glowing glyphs; 2560×720; PNG with alpha; Optional; Placeholder: Procedural glowing glyph monoliths; No special licensing.
  3. `foreground_pillars.png` — Shockwave laboratory towers; 1920×720; PNG with alpha; Optional; Placeholder: Procedural purple conduit pillars; No special licensing.

### Level 05: Frozen Facility
- **Directory**: `assets/environments/level_05_frozen/`
- **Files**:
  1. `sky.png` — Arctic blizzard sky with frozen aurora; 1920×1080; PNG/JPG; Optional; Placeholder: Procedural cyan aurora sky; No special licensing.
  2. `midground.png` — Glaciated Cybertron military bunker; 2560×720; PNG with alpha; Optional; Placeholder: Procedural jagged ice spires; No special licensing.

### Level 06: Decepticon Base (Boss: Soundwave)
- **Directory**: `assets/environments/level_06_decepticon_base/`
- **Files**:
  1. `sky.png` — Cavernous interior fortress ceiling with communications arrays; 1920×1080; PNG/JPG; Optional; Placeholder: Procedural dark metallic ceiling grid; No special licensing.
  2. `midground.png` — Massive server racks, holographic audio visualizers; 2560×720; PNG with alpha; Optional; Placeholder: Procedural pulsing soundwave bars; No special licensing.

### Level 07: Aerial Battlefield
- **Directory**: `assets/environments/level_07_aerial/`
- **Files**:
  1. `sky.png` — High stratosphere clouds, dogfight vapor trails; 1920×1080; PNG/JPG; Optional; Placeholder: Procedural dynamic cloud layers; No special licensing.
  2. `midground.png` — Floating anti-gravity battle platforms; 2560×720; PNG with alpha; Optional; Placeholder: Procedural floating carriers; No special licensing.

### Level 08: Destroyed Metropolis (Boss: Starscream)
- **Directory**: `assets/environments/level_08_metropolis/`
- **Files**:
  1. `sky.png` — Burning metropolis evening sky with smoke columns; 1920×1080; PNG/JPG; Optional; Placeholder: Procedural flaming crimson dusk; No special licensing.
  2. `midground.png` — Collapsing capital spires, Decepticon seeker silhouettes; 2560×720; PNG with alpha; Optional; Placeholder: Procedural collapsed spires; No special licensing.

### Level 09: Cybertron Outskirts (Elite Gauntlet)
- **Directory**: `assets/environments/level_09_outskirts/`
- **Files**:
  1. `sky.png` — Toxic storm clouds, purple Energon lightning strikes; 1920×1080; PNG/JPG; Optional; Placeholder: Procedural storm flashing; No special licensing.
  2. `midground.png` — Decimated fortifications, debris fields; 2560×720; PNG with alpha; Optional; Placeholder: Procedural trench fortifications; No special licensing.

### Level 10: Final Battlefield (Boss: Megatron)
- **Directory**: `assets/environments/level_10_final_battlefield/`
- **Files**:
  1. `sky.png` — The Core of Cybertron: Deep molten Energon chasm, dark metallic core sphere; 1920×1080; PNG/JPG; Optional; Placeholder: Procedural glowing Cybertronian planetary core; No special licensing.
  2. `midground.png` — Cybertronian throne citadel and towering war monuments; 2560×720; PNG with alpha; Optional; Placeholder: Procedural dark throne citadel; No special licensing.

---

## 3. Music Media Specification

The current game uses exactly two external looping background-score files. Both must be OGG Vorbis files (44.1 kHz stereo is the supplied format). These files are not optional and there is no procedural background-music fallback.

| Filename | Required directory | Active states |
|---|---|---|
| `Optimus.ogg` | `assets/music/menu/` | Opening cinematic, main menu, level selection, settings, pause menu, and other non-gameplay states. |
| `Optimus_Prime_vs._Megatron.ogg` | `assets/music/campaign/` | Active gameplay only. |

Victory and defeat result screens are completely silent. The gameplay score is mixed lower than the sound effects to preserve combat readability; the menu score is given greater prominence. Music and SFX volume settings and the master-volume setting remain in effect.

**Licensing notice:** Before commercial distribution, verify that you hold the necessary synchronization, master-use, and distribution rights for both tracks. Uploading or possessing a file does not itself grant commercial-use permission.

---

## 4. Voice Lines Media Specification

- **Accepted Formats**: `.wav` or `.ogg` (Mono or Stereo, 44.1 kHz).
- **Directory**: `assets/voice/`
- **Optional**: All voice triggers have subtitle and visual notification fallbacks.

| Filename | Directory | Speaker | Trigger | Line / Transcript | Licensing |
|---|---|---|---|---|---|
| `transform_rollout.wav` | `assets/voice/optimus/` | Optimus Prime | Level 1 Start / Transformation | *"Autobots, transform and roll out!"* | Hasbro / Peter Cullen (or voice actor) |
| `one_shall_stand.wav` | `assets/voice/optimus/` | Optimus Prime | Level 10 Confrontation | *"One shall stand, one shall fall."* | Hasbro / Peter Cullen |
| `hold_the_line.wav` | `assets/voice/optimus/` | Optimus Prime | Low HP / Wave start | *"We will hold this ground."* | Hasbro / Peter Cullen |
| `megatron_peace_tyranny.wav` | `assets/voice/megatron/` | Megatron | Level 10 Intro | *"Peace through tyranny, Prime!"* | Hasbro / Frank Welker |
| `megatron_overdrive.wav` | `assets/voice/megatron/` | Megatron | Phase 2 Transition | *"I will crush you with my bare hands!"* | Hasbro / Frank Welker |
| `shockwave_logical.wav` | `assets/voice/other_characters/` | Shockwave | Boss Intro Level 4 | *"Your destruction is the only logical outcome."* | Hasbro / Voice Actor |
| `soundwave_superior.wav` | `assets/voice/other_characters/` | Soundwave | Boss Intro Level 6 | *"Soundwave superior. Autobots inferior."* | Hasbro / Voice Actor |
| `starscream_lord.wav` | `assets/voice/other_characters/` | Starscream | Boss Intro Level 8 | *"Cybertron belongs to me now, Prime!"* | Hasbro / Voice Actor |

---

## 5. UI Artwork and Logos

- **Directory**: `assets/ui/`
- **Format**: PNG with 32-bit RGBA transparency.

| Filename | Directory | Purpose | Required Dimensions | Optional? | Placeholder Status |
|---|---|---|---|---|---|
| `game_logo.png` | `assets/ui/logos/` | High-res title header | 960×320 | Yes | Procedural vector lettering with drop-shadow |
| `autobot_insignia.png` | `assets/ui/icons/` | Autobot emblem for HUD & menu | 128×128 | Yes | Procedural geometric shield crest |
| `decepticon_insignia.png` | `assets/ui/icons/` | Decepticon emblem for Boss HUD | 128×128 | Yes | Procedural geometric horned crest |
| `lock_icon.png` | `assets/ui/icons/` | Level select lock status badge | 64×64 | Yes | Procedural padlock vector |
| `check_icon.png` | `assets/ui/icons/` | Level completed badge | 64×64 | Yes | Procedural checkmark vector |
| `campaign_map_bg.png` | `assets/ui/artwork/` | Level selection screen backdrop | 1280×720 | Yes | Procedural Cybertron planetary grid |

