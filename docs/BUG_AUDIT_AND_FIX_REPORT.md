# Transformers Brawler — Source Audit and Bug-Fix Report

Audit target: `Transformers.zip` (uploaded 2026-11-13)

## Executive summary

The archive contains a substantial C++17/Raylib project with 6,330 lines across 39 source/build files, plus assets directory scaffolding, a prebuilt Windows executable, object files, and save-data copies.

The source was inspected and the following confirmed issues were corrected:

1. **Save checksum depended on struct padding.** The original checksum hashed `sizeof(SaveData)` raw bytes. C++ padding bytes are not a stable serialization contract and can differ between builds/compilers, potentially rejecting a valid save. The checksum now hashes each persisted field individually.
2. **Save format did not validate trailing/truncated data robustly.** Loading now checks that the checksum is present and that no unexpected trailing bytes remain.
3. **Persisted values were insufficiently sanitized.** Loaded progression, upgrade levels, volume, palette, display, brightness, difficulty, and camera/effect settings are now clamped to safe ranges.
4. **Replay completion could repeatedly award Energon Cores.** Replaying an already completed level previously awarded the completion currency again. Currency is now awarded only on first completion; high scores can still improve on replay.
5. **Settings changes did not apply audio changes immediately.** The settings UI persisted changes, but `Game::Update` only reapplied audio settings on leaving the settings screen. Audio toggles and volume changes now call `ApplySettings()` as soon as a change is reported.
6. **Build configuration was tied to a standalone `C:/raylib` layout.** The Makefile now prefers `pkg-config` (suitable for MSYS2 UCRT64 Raylib installations) and retains the standalone Raylib fallback.

## Validation performed

- Compiled `SaveSystem.cpp` with GCC 14.2 using a minimal Raylib header stub (SaveSystem only needs the Raylib include transitively through settings).
- Ran a focused save/progression test covering initial unlock state, Level 1 completion unlocking Level 2, replay not farming currency, high-score improvement, and save/load persistence. Result: PASS.
- Confirmed the archive contains 17 `.cpp` compilation units and all are listed in the Makefile.
- Confirmed the supplied container does not have Raylib installed (`pkg-config --modversion raylib` reports package not found). Therefore a full application build, link, graphical runtime test, audio test, and Windows/MSYS2 integration test could not be executed here. No claim of a full-game build pass is made.

## Additional findings / risks (not represented as verified runtime failures)

- The archive contains prebuilt `.exe` and `.o` files. These can be stale relative to source and were excluded from the refined source ZIP; rebuild locally.
- The project contains two save-data files (`save_data.dat` and `bin/save_data.dat`). The current save path is relative (`save_data.dat`), so the working directory determines which file is used. Run the executable from the project root. A future refinement should move saves to a stable per-user writable directory.
- The opening sequence is currently a procedural 2D starfield/text presentation, not the requested high-end morph-style cinematic. This is a feature-quality gap, not a defect in the existing skip flow.
- Character visuals are currently procedural 2D drawings; the requested detailed Raylib procedural 3D Bayverse-level character system is not present yet.
- External music loading checks a small fixed set of filenames. Missing music falls back to synthesized procedural music. The requested licensed Transformers: Prime score is not included.
- The settings UI exposes a contrast value in `GameSettings`, but the current settings screen does not expose a contrast control. Display resolution/fullscreen settings are stored but not applied by the current UI.
- The campaign structure and level selection exist, but this audit did not verify all ten level experiences as complete production-ready levels. The project should be treated as an evolving game foundation, not a finished commercial product.
- `StartLevel` currently relies on its callers to enforce progression. The level-selection UI does enforce unlock checks; a future defensive improvement should also validate unlock state inside the level-start boundary.
- The combat hit registry uses a packed 32-bit key with 16-bit target IDs and clears the registry after 500 entries. This is a collision/long-fight risk if IDs exceed 16 bits or an unusually long attack accumulates many targets.
- Gameplay and audio use the global C `rand()` generator. This is adequate for basic variation but is not deterministic and is harder to test/replay than a dedicated seeded RNG.

## Files changed

- `src/core/SaveSystem.cpp`
- `src/core/Game.cpp`
- `Makefile`

## Save compatibility note

The save format version was advanced from 1 to 2 because checksum semantics changed. Existing version-1 save files will be treated as incompatible and reset to defaults by the current loader. Back up any valuable existing save before running the refined build.

## Local verification (MSYS2 UCRT64)

From the project root:

```bash
make clean
make
./bin/transformers_brawler.exe
```

If Raylib is installed in UCRT64 through MSYS2, ensure `pkg-config --modversion raylib` succeeds in that terminal before building.

## Final status

The confirmed, safely fixable issues listed above have been patched. A focused save/progression regression test passed. A complete Raylib application build and interactive QA remain pending because Raylib and a Windows runtime are unavailable in this audit environment.
