# Background Music Playback Hotfix

## Problem addressed
The previous implementation relied exclusively on paths relative to the process working directory and treated a requested track as active without checking whether the Raylib stream was actually playing. This could leave the game silent when launched with a different working directory or after a stream failed to start.

## Changes
- Resolve both external music assets from the project working directory or from the executable directory (including `bin/` launches).
- Initialize Raylib `Music` handles to zero before loading.
- Only skip a same-track request when the requested stream is actually playing.
- Stop both loaded streams when changing tracks, including previously paused streams.
- Log audio-device readiness, successfully loaded asset paths, effective music volume, and each track start to the console.
- Keep the existing state routing, external music filenames, sound effects, and silent victory/defeat states unchanged.

## Required asset paths
- `assets/music/menu/Optimus.ogg`
- `assets/music/campaign/Optimus_Prime_vs._Megatron.ogg`

## Runtime verification
A Raylib build and playback test must still be run in the user's MSYS2 UCRT64 environment. On startup, the console should report that the audio device is ready, both scores loaded, and the menu score started. If the effective music volume is `0`, check Settings > Audio: Music Output must be enabled, Music Volume must be above zero, and Master Volume must be above zero.
