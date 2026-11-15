# Campaign progression and completion-screen refinement

## Scope

This refinement is intentionally additive. Existing player, combat, enemy, boss, arena, settings, save-data layout, and level-selection implementations were retained. The changes are limited to the level-start guard, the victory-state flow, and a new dedicated completion UI.

## Changes

1. `Game::StartLevel` now returns success/failure and rejects level numbers outside 1-10 or levels not unlocked in `SaveData`. All in-game call sites only enter `PLAYING` when the start succeeds.
2. Successful completion of every arena level enters the same `VICTORY` state and displays `MissionCompletionUI`, rather than relying on the generic menu's partial victory panel.
3. The report shows level number/name/subtitle, enemies eliminated, combat score, saved level best, completion time, and the Energon Core amount awarded during this run.
4. When the next sequential level is unlocked, the report offers `NEXT UNLOCKED LEVEL`, `CAMPAIGN MAP / LEVEL SELECT`, `REPLAY THIS LEVEL`, and `RETURN TO MAIN MENU`.
5. Level 10 displays `CAMPAIGN COMPLETE` and omits the nonexistent next-level action.
6. The result screen is keyboard-only: Up/Down selects an action and Enter/Space confirms it. Mouse interaction is not supported. The next-level action checks unlock status again immediately before calling the guarded level-start function.
7. The completion timer resets on each level start and pauses naturally outside active gameplay. The reward display uses the actual before/after currency difference, so a replay correctly displays zero additional completion cores when the one-time reward was already claimed.

## Files changed

- `src/core/Game.hpp`
- `src/core/Game.cpp`
- `src/ui/MissionCompletionUI.hpp` (new)
- `src/ui/MissionCompletionUI.cpp` (new)
- `Makefile`
- `README.md`
- `tests/CAMPAIGN_PROGRESSION_TEST_PLAN.md` (new)

## Validation performed

- `MissionCompletionUI.cpp` passed a GCC C++17 syntax-only compile using a minimal Raylib API stub.
- Source-level call-site review confirms all `StartLevel` callers check the returned result, except initial Level 1 setup where the request is a known valid default.
- The new UI source is included in the Makefile source list.
- A full application build and runtime test could not be performed in this environment because Raylib and its `pkg-config` package are not installed. The full Windows/MSYS2 UCRT64 build remains required before treating the application as runtime-verified.

## Compatibility and preservation

- No `SaveData` fields, serialization layout, save version, or checksum logic were changed by this refinement.
- Existing `save_data.dat` progression and settings are therefore not intentionally invalidated by these changes.
- Existing game-over, pause, settings, audio, combat, and level-select implementations remain in place.

## Required local verification

Follow `tests/CAMPAIGN_PROGRESSION_TEST_PLAN.md` after building with Raylib in MSYS2 UCRT64. Back up `save_data.dat` before testing progression from a clean campaign.
