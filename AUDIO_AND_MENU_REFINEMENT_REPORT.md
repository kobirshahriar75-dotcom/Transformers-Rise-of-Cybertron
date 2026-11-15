# Audio and Menu Interaction Refinement

## Implemented

### External background scores
- `assets/music/menu/Optimus.ogg`: opening cinematic, main menu, level selection, settings, pause menu, and other non-gameplay states.
- `assets/music/campaign/Optimus_Prime_vs._Megatron.ogg`: active gameplay only.
- Both tracks loop as Raylib `Music` streams.
- Background music is now external-file-only. The procedural background music stream and its fallback generation were removed. Missing/invalid files produce explicit console errors.
- Existing one-shot sound effects remain separate and unchanged in source generation/triggering.

### Mix policy
- Gameplay background-score gain: 0.70, before the user's music/master volume settings.
- Non-gameplay background-score gain: 0.90, before the user's music/master volume settings.
- Existing SFX volume and master-volume settings remain active. The gameplay score is deliberately reduced relative to the SFX bus; the menu score is more prominent because it is used in non-combat states.

### Result-screen silence
- On both victory and defeat, the current music stream is stopped and all currently playing one-shot SFX are stopped.
- `MusicTrack::NONE` is maintained while either result screen is displayed.
- Victory and defeat result screens are keyboard-only. Up/Down selects an action; Enter/Space confirms it. J remains a gameplay attack key and cannot activate a result-screen option.
- Mouse polling, pointer-based selection, and mouse-button activation have been removed from the game source.

### Menu selection defaults
- Main menu: Continue Campaign when progress has unlocked Level 2 or later; otherwise New Campaign.
- Pause menu: Resume Combat.
- Defeat screen: Retry Sector.
- Victory screen: Next Unlocked Level when available; otherwise Campaign Map / Level Select.
- Menu selection is reset on state entry and also at the first draw after a state transition, so the default highlight is correct immediately.

## Supplied audio files inspected

| File | Duration | Format | Sample rate | Channels |
|---|---:|---|---:|---:|
| `Optimus.ogg` | 2:52.788 | OGG Vorbis | 44.1 kHz | Stereo |
| `Optimus_Prime_vs._Megatron.ogg` | 3:34.061 | OGG Vorbis | 44.1 kHz | Stereo |

Both files were successfully decoded for metadata and short playback inspection, then copied into their required project folders.

## Verification performed

- Confirmed both OGG files decode and have the expected stereo/44.1 kHz format.
- Confirmed the project contains both files at the exact paths used by `SoundSystem`.
- The original audio/menu refinement was later superseded by the final keyboard-only conversion documented in `FINAL_REFINEMENT_REPORT.md`.
- Confirmed victory and defeat transitions set `MusicTrack::NONE` and stop active SFX.
- Confirmed the pause state selects the menu track and resuming combat selects the gameplay track.
- Confirmed archive integrity and required assets are present.

## Not yet verified

A full Raylib compile and interactive runtime test were not possible in this environment because Raylib is not installed/configured here. Please run the build and the manual checks in `AUDIO_AND_MENU_TEST_PLAN.md` using the project's MSYS2 UCRT64 environment before treating the changes as runtime-approved.
