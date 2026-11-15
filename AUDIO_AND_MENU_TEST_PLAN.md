# Audio and Menu Refinement — Manual Test Plan

Run from the project root in the MSYS2 UCRT64 terminal.

```bash
make clean
make
./bin/transformers_brawler.exe
```

## Required tests

1. **Startup/menu score** — opening cinematic and main menu play `Optimus.ogg`; no gameplay track is audible.
2. **Menu transitions** — level selection and settings continue to use `Optimus.ogg`.
3. **Gameplay score** — starting any level stops the menu score and plays `Optimus_Prime_vs._Megatron.ogg`.
4. **Gameplay mix** — test melee, blaster, transformation, enemy impacts, explosions, and boss sounds. SFX must remain clearly audible without making the score disappear. Adjust the game's music/SFX settings if needed; report which side is too loud.
5. **Pause** — opening pause stops the gameplay score and plays `Optimus.ogg`; resuming stops the menu score and returns to the gameplay score.
6. **Victory silence** — complete a level. Once the victory report appears, there must be no music and no SFX.
7. **Victory keyboard focus** — J must do nothing. Up/Down or W/S must move the focus; Enter/Space must activate only the highlighted action. Escape must not activate a result action.
8. **Victory keyboard progression** — use Up/Down to highlight `NEXT UNLOCKED LEVEL`, then Enter/Space. Only the explicitly focused action should start the next unlocked level.
9. **Victory alternate actions** — navigate to Campaign Map, Replay, and Main Menu with Up/Down and activate each with Enter/Space.
10. **Defeat silence** — lose a level. The defeat report must be completely silent.
11. **Defeat keyboard focus** — J must do nothing. Up/Down or W/S must move the focus; Enter/Space must activate only the highlighted action. Escape must not activate a result action.
12. **Defeat keyboard actions** — navigate to Retry Sector and Tactical Map with Up/Down, then activate each with Enter/Space.
13. **Default selection** — enter the main menu from the opening cinematic with a fresh save (New Campaign selected), then with progress (Continue Campaign selected); pause from gameplay, win a level, and lose a level. Confirm the intended default button is highlighted immediately on the first displayed frame.
14. **Repeated transitions** — repeat pause/resume and win/replay cycles to ensure tracks do not overlap or retain the wrong selection.
15. **Shutdown** — exit normally and confirm there are no audio-device or stream-unload errors.

## Expected defaults

- Main menu: Continue Campaign if campaign progress exists; otherwise New Campaign.
- Pause: Resume Combat.
- Victory: Next Unlocked Level (or Campaign Map / Level Select if no next level exists).
- Defeat: Retry Sector.
