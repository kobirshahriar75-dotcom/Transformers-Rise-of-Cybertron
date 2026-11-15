# Campaign progression and mission completion regression tests

Run these tests in a clean local build using the MSYS2 UCRT64 environment. Back up `save_data.dat` before testing.

## Progression and validation

1. Start with a fresh save. Confirm Level 1 is available and Levels 2-10 are locked.
2. Attempt to start Level 2 from the level-select interface. It must not start.
3. Complete Level 1. Confirm the completion report appears and Level 2 becomes unlocked.
4. Select **NEXT UNLOCKED LEVEL**. Confirm Level 2 starts directly, with a reset player, arena, projectiles, particles, combat state, and timer.
5. Complete Level 2. Confirm Level 3 unlocks and the same report layout appears.
6. Complete through Level 10. At each level, verify only the immediately following campaign level is newly unlocked.
7. Attempt a locked level through any available start route. `Game::StartLevel` must reject it.

## Completion-screen actions

8. On every level, verify the report includes level identity, enemies eliminated, score, best score, completion time, and Energon Cores earned.
9. Use Up/Down to highlight **NEXT UNLOCKED LEVEL**, then Enter/Space. Verify it starts the next level only when that level is unlocked.
10. Use keyboard Up/Down and Enter/Space to activate the same options.
11. Choose **CAMPAIGN MAP / LEVEL SELECT**. Confirm the level-select screen opens and unlocked levels remain replayable.
12. Choose **REPLAY THIS LEVEL**. Confirm the same level restarts and completion rewards are not farmed on replay.
13. Choose **RETURN TO MAIN MENU**. Confirm the main menu opens.
14. Complete Level 10. Confirm the report says **CAMPAIGN COMPLETE**, does not offer a nonexistent Level 11, and still offers campaign map, replay, and main-menu actions.

## Existing-system regression

15. Verify pause/resume, restart, game-over retry, settings, audio, and save/load still work.
16. Quit after unlocking several levels, relaunch, and confirm progression persists.
17. Replay a previously completed level. Confirm its high score can improve but its one-time completion reward is not awarded again.
