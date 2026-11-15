# Transformers: Cybertron Brawler (30-Day Vertical Slice Prototype)

A fast-paced, 2D side-scrolling action brawler developed in **C++17** and **Raylib 6.2**. The player controls **Optimus Prime** defending Cybertron against Decepticon forces and confronting **Megatron** in a climactic boss showdown.

---

## Official Campaign Transformation Rules

- **Levels 1-6 and 9-10:** Optimus Prime has three selectable modes: Energon Blade (default at every level start), Plasma Cannon, and Truck.
- **Levels 7-8:** These aerial-combat levels lock Optimus Prime into one Jetpack-powered configuration. Standard mode switching is disabled, and holding `J` or `Z` fires continuous plasma bolts. `K` or `X` triggers the aerial overload special.
- Starting or replaying any level resets Optimus Prime to Energon Blade before the level-specific aerial configuration is applied.

## 🎮 Core Gameplay & Features

### 1. Playable Optimus Prime with 3 Transformation Modes
Switch seamlessly between three distinct combat and mobility modes using `[C]` or `[1, 2, 3]`:
- **Energon Blade Mode (`[1]`)**: High-mobility melee brawler.
  - **3-Hit Combo (`[J]` / `[Z]`)**: Horizontal Slash -> Rising Uppercut -> Overhead Leaping Energon Axe Finisher.
  - **Energon Wave Slash (`[K]` / `[X]`)**: Unleashes a long-range crescent energy blade wave across the arena (costs 25 Energy).
  - **Acrobatic Thrusters**: Double-jump capability for aerial agility.
- **Plasma Cannon Mode (`[2]`)**: Tactical ranged suppression.
  - **Ion Blaster Shot (`[J]` / `[Z]`)**: High-velocity glowing plasma bolts that detonate upon impact with recoil kickback.
  - **Charged Overload Burst (`[K]` / `[X]`)**: Dual explosive energy cannons delivering high damage and screen shake (costs 30 Energy).
- **Truck Alt-Mode (`[3]`)**: High-speed vehicular destruction.
  - **Momentum-Based Ramming**: Speeds up to **820 px/s**, plowing through enemies and knocking them airborne!
  - **Nitro / Rocket Boost (`[SPACE]` or `[K]` / `[X]`)**: Super-accelerates with blazing flame exhaust trails.
  - **Big-Rig Air Horn (`[H]` or `[J]`)**: Sounds authentic dual-tone truck horn.

### 2. Visceral Combat System ("Juice")
- **Active Hitboxes & Directional Hit Detection**: Precise frame windows and knockback vectors.
- **Hit-Stop Micro-Freezes**: Satisfying momentary freeze on impact (0.04s - 0.12s) to convey weight and power.
- **Trauma-Based Screen Shake**: Quadratic decay trauma system that shakes the camera dynamically on heavy strikes and explosions.
- **Particle & Visual FX**:
  - Sparks bouncing off metallic surfaces with gravity.
  - Expanding shockwave rings on impacts.
  - Smoke puffs from exhaust stacks and cannon muzzles.
  - Floating damage numbers with critical strike highlights.

### 3. Decepticon Enemy AI & Boss Encounter
- **Decepticon Drone (Seeker Scout)**:
  - Fast-moving foot soldier armed with purple vibro-blades.
  - Patrols arena, detects player, pursues, and performs telegraphed strikes.
  - Fires ranged laser darts when positioned at medium range.
- **Decepticon Brute (Heavy Enforcer)**:
  - Armored heavyweight mech with high knockback resistance.
  - Heavy iron punches and devastating ground slams creating floor shockwaves.
- **Megatron Boss Encounter**:
  - Full boss health bar with segmented phase markers.
  - **Attack Patterns**:
    1. *Fusion Cannon Blast*: Concentrated purple plasma death ray with charge-up telegraph.
    2. *Energon Mace Slam*: Massive chained flail sweep that smashes the ground.
    3. *Jet Thruster Dash*: High-speed ramming flight across the arena.
  - **Phase 2 (Overdrive)**: Below 50% HP, Megatron erupts with purple electrical lightning aura, shorter telegraphs, and increased ferocity!

### 4. Cybertronian Atmosphere & Parallax
- Multi-layer parallax background:
  - Deep violet Cybertron sky with metallic Primus moon.
  - Distant spires with pulsing neon windows (0.1x parallax).
  - Industrial factories and glowing purple Energon conduits (0.35x parallax).
  - Heavy hazard support pylons (0.65x parallax).
  - Riveted steel roadway with hazard chevrons and glowing neon edge trim (1.0x world space).

### 5. Built-in Procedural Audio Engine
100% self-contained sound effects and music synthesis in real time via Raylib's `Wave` and `AudioStream`:
- Slash whooshes, metal impact clangs, blaster pews, explosive booms.
- Iconic 5-tone robotic transformation audio sequence.
- Authentic semi-truck horn and turbine engine roar.
- Continuous 125 BPM Cybertronian synthwave combat soundtrack with driving bass, energon arpeggios, kick, and snares.

---

## ⌨️ Controls Reference

| Action | Primary Key | Secondary Key |
| :--- | :--- | :--- |
| **Move Left / Right** | `A` / `D` | `Left` / `Right` Arrow |
| **Jump / Double-Jump** | `W` | `Space` / `Up` Arrow |
| **Melee Attack / Fire Blaster** | `J` | `Z` (hold for continuous plasma in Levels 7-8) |
| **Special Attack / Charged Shot** | `K` | `X` |
| **Cycle Transformation** | `C` | `Left Shift` |
| **Direct Mode Select** | `1` (Blade) \| `2` (Cannon) \| `3` (Truck) | - |
| **Truck Turbo Boost** | `Space` (while in truck mode) | `K` / `X` |
| **Truck Air Horn** | `H` | `J` (while in truck mode) |
| **Pause / Resume** | `P` | `Escape` |

---

## 🛠️ Build and Run Instructions

### Prerequisites
- **Compiler**: MinGW-w64 GCC with C++17 support (e.g. `C:\raylib\w64devkit\bin\g++.exe`)
- **Library**: Raylib 6.2 headers and static library (`C:\raylib\raylib\src\libraylib.a`)
- **Build Tool**: GNU Make (`make`)

### Compiling via Make
Open MSYS2 UCRT64, PowerShell, or CMD in the project folder and run:
```bash
make
```

### Running the Executable
```bash
./bin/transformers_brawler.exe
```

Or run via make target:
```bash
make run
```

### Visual Studio Code
- Open `C:\Users\HP\.gemini\antigravity\scratch\transformers_brawler` in VS Code.
- Press `Ctrl+Shift+B` to trigger the build task.
- Press `F5` to build and launch with the GDB debugger.

---

## 📁 Project Architecture

```
transformers_brawler/
├── Makefile                          # GNU Make build configuration
├── README.md                         # Documentation and controls guide
├── .vscode/
│   ├── tasks.json                    # VS Code build and run tasks
│   └── launch.json                   # GDB debugger configuration
├── src/
│   ├── main.cpp                      # Program entry point
│   ├── core/
│   │   ├── Constants.hpp             # Dimensions, physics tuning, faction types, colors
│   │   ├── Game.hpp / .cpp           # Main game loop, state transitions, hit-stop
│   │   ├── CameraManager.hpp / .cpp  # Dynamic tracking, boundary clamps, trauma shake
│   │   └── SoundSystem.hpp / .cpp    # Procedural sound FX & dynamic BGM audio stream
│   ├── entities/
│   │   ├── Entity.hpp                # Base entity class with hurtbox and physics
│   │   ├── Player.hpp / .cpp         # Optimus Prime (3 modes, combos, rendering)
│   │   ├── Enemy.hpp / .cpp          # Decepticon Drone & Brute AI states
│   │   ├── Boss.hpp / .cpp           # Megatron (Fusion Cannon, Mace, Jet Dash, Phase 2)
│   │   └── Projectile.hpp / .cpp     # Plasma bolts, energon waves, fusion orbs
│   ├── combat/
│   │   ├── Hitbox.hpp                # Hitbox and hurtbox structures
│   │   └── CombatSystem.hpp / .cpp   # Collision detection, damage, knockback resolution
│   ├── effects/
│   │   └── ParticleSystem.hpp / .cpp # Sparks, smoke, shockwave rings, damage numbers
│   ├── world/
│   │   ├── Arena.hpp / .cpp          # 4-Wave progression & enemy spawning
│   │   └── ParallaxBackground.hpp / .cpp # Multi-layered Cybertron cityscape
│   └── ui/
│       ├── HUD.hpp / .cpp            # Player HP, Energon gauge, mode badge, Boss bar
│       └── MenuSystem.hpp / .cpp     # Main menu, pause, game over, victory, controls modal
```

## Audit and rebuild

See `docs/BUG_AUDIT_AND_FIX_REPORT.md` for the source audit, confirmed fixes, validation results, and known remaining risks. Rebuild from source in the MSYS2 UCRT64 terminal with `make clean && make`, then run `./bin/transformers_brawler.exe`.

## Campaign completion update

The campaign flow now uses a dedicated mission-completion report for every successfully completed level. The report provides level identity, combat results, completion time, best score, and the Energon Core reward earned on that run. When the next sequential level is unlocked, **NEXT UNLOCKED LEVEL** starts it directly. The report also offers campaign map, replay, and main-menu actions. Level 10 displays a campaign-completion state without a Level 11 action.

`Game::StartLevel` now validates the requested level against saved campaign progression, providing a final guard even if a caller bypasses the level-select UI. See `tests/CAMPAIGN_PROGRESSION_TEST_PLAN.md` for the regression checklist.
