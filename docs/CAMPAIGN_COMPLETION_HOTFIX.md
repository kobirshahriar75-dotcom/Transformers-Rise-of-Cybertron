# Campaign Completion Reliability Hotfix

## Why the previous refinement was insufficient

The previous implementation depended on the arena's wave-update branch to set the victory flag. That made the completion flow dependent on the timing/order of wave progression and combat resolution. The completion UI itself existed, but the final objective was not independently evaluated after combat had applied its damage/death results.

## Conservative correction

- Added `Arena::TryCompleteLevel()` as the single, idempotent final-objective check.
- The check requires the final campaign wave to have spawned.
- It requires every remaining enemy to be defeated.
- Boss levels additionally require the configured boss to exist and be defeated.
- The victory flag is latched, and the sector-secured banner is set once.
- `Arena::Update()` uses this shared check instead of a separate boss/non-boss victory branch.
- `Game::Update()` calls the same check immediately after combat resolution. This recognizes the final kill/boss defeat without waiting for a later wave-update pass.
- Existing `GameState::VICTORY` and `MissionCompletionUI` flow remains the single completion-window path for all ten levels.

No combat damage, enemy AI, boss behavior, save structure, level definitions, or menu layout was intentionally redesigned.

## Verification limits

Static source checks were performed on the ten-level database and shared completion route. A full Raylib compile and interactive gameplay run were not possible in the authoring environment because Raylib/MinGW is not installed there. Run the MSYS2 UCRT64 build and the manual regression plan before treating the runtime behavior as verified.
