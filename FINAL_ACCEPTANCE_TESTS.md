# Final Acceptance Tests — Keyboard-Only and Visual Refinement

Back up `save_data.dat` before testing. Run from the MSYS2 UCRT64 terminal at the project root.

## Build and launch

```bash
pkg-config --modversion raylib
make clean
make
./bin/transformers_brawler.exe
```

If `pkg-config` is unavailable but Raylib is installed in the MSYS2 UCRT64 prefix, the Makefile will attempt the standard `C:/msys64/ucrt64` include and library directories.

## Keyboard-only acceptance

1. Move the mouse throughout every screen. Confirm that no selection, hover effect, action, slider, or menu state responds to it.
2. Opening cinematic: Space, Enter, and Escape each skip the cinematic. Confirm the displayed hint matches the keys.
3. Main menu: navigate all seven entries using arrows/WASD; activate each with Enter/Space. Verify Controls and Credits can be closed with Enter/Space and Escape.
4. Settings: use Q/E to visit all three tabs. Navigate every row with Up/Down. Adjust sliders with Left/Right or A/D; toggle each switch; reach Save & Return with Down and activate with Enter/Space. Verify Escape also saves and returns.
5. Campaign map: navigate all ten cards with arrows/WASD. Locked sectors must remain unlaunchable. Press Tab to focus Back and Enter/Space to return. Verify Escape/Backspace also returns.
6. Pause: press P or Escape during gameplay. Navigate Resume, Restart, and Main Menu with Up/Down and activate with Enter/Space.
7. Defeat: navigate Retry and Tactical Map with Up/Down; activate with Enter/Space.
8. Victory: navigate every available action with Up/Down and activate with Enter/Space. Press J while the report is open; it must not activate Next Level or any other option.
9. Confirm every screen displays a visible selected/focus treatment and keyboard instructions where applicable.

## Gameplay and campaign regression

10. Verify movement, jump, attacks, special attack, transformation, direct mode selection, truck controls, and pause retain their existing bindings.
11. Complete each of the ten campaign levels and confirm the appropriate completion report appears after the final objective.
12. Confirm Level 10 reports campaign completion and does not offer Level 11.
13. Confirm Next Level starts only the immediately following unlocked level; Campaign Map, Replay, and Main Menu each perform their own action.
14. Confirm replay does not award one-time completion currency again and does not corrupt progression.
15. Confirm save/load preserves unlocked levels, completion flags, scores, and settings.

## Audio regression

16. Confirm `Optimus.ogg` plays in the opening cinematic and non-gameplay screens.
17. Confirm `Optimus_Prime_vs._Megatron.ogg` plays only during active gameplay.
18. Confirm pause changes to the non-gameplay score and resuming restores the gameplay score.
19. Confirm victory and defeat reports are completely silent, including no music and no sound effects.
20. Verify master, music, and SFX settings still affect the intended audio buses.

## Visual and stability review

21. Review main menu, campaign map, settings, pause, defeat, victory, and gameplay HUD at the target resolution.
22. Confirm animated accents do not obscure text, character sprites, attacks, health bars, or mission information.
23. Confirm no flickering, severe frame-rate drop, layout clipping, or unreadable contrast occurs.
24. Play through at least one standard level and one boss level without crashes or state-transition failures.

Record any failed test with the screen/state, exact key sequence, and terminal output.

## Soundwave / Level 6 balance regression
- Confirm Soundwave's second attack visibly fires two sonic discs and does not create a close-range melee hitbox.
- Confirm a single sonic disc deals 18 damage, not 25.
- Confirm each shoulder-salvo disc deals 16 damage, not 24.
- Confirm attack telegraphs last 0.70 seconds in phase one and 0.50 seconds in phase two.
- Confirm the post-attack cooldown is 1.90 seconds in phase one and 1.35 seconds in phase two.
- Test at close, medium, and long range; confirm the player has a fair opportunity to react and evade.
- Confirm other bosses retain their previous attack damage and hitbox behaviour.

## Level 7 Aerial Battlefield / Level 8 Starscream Regression

See `tests/LEVEL_7_AERIAL_COMBAT_REGRESSION.md` for the complete aerial mission test. At minimum, verify that Level 7 starts in Jetpack Mode, J/Z fires plasma bolts, K/X fires the 30-energy aerial overload, and the four seeker waves contain 3/4/5/6 enemies. Confirm Level 8 retains its 4/5/6/7 seeker waves and Starscream encounter.

Run the source-level regression check from the project root:

```bash
python tools/verify_aerial_balance.py
```
