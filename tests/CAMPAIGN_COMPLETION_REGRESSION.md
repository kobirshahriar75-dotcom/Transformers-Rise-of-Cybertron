# Campaign Completion Regression Checklist

Run each level from the campaign map and confirm the same mission-completion report appears after its final objective is cleared.

| Level | Final objective | Expected result |
|---|---|---|
| 1 | Clear final standard wave | Mission Completion report |
| 2 | Clear final standard wave | Mission Completion report |
| 3 | Clear final standard wave | Mission Completion report |
| 4 | Defeat Shockwave | Mission Completion report |
| 5 | Clear final standard wave | Mission Completion report |
| 6 | Defeat Soundwave | Mission Completion report |
| 7 | Clear final aerial wave | Mission Completion report |
| 8 | Defeat Starscream | Mission Completion report |
| 9 | Clear final elite wave | Mission Completion report |
| 10 | Defeat Megatron | Campaign Complete report; no Level 11 button |

For every level also verify:

- The report appears after the final objective, not after an intermediate wave.
- `NEXT UNLOCKED LEVEL` appears when a next level exists and is unlocked.
- Selecting it starts exactly the next level.
- Campaign Map, Replay, and Main Menu work and do not accidentally start another level.
- Completing a level registers progression once; leaving/re-entering the report does not duplicate rewards.
- Defeat still opens the existing Game Over flow and never reports a mission victory.
- Replaying an already-unlocked level remains possible.

## Build (MSYS2 UCRT64)

```bash
pkg-config --modversion raylib
make clean
make
./bin/transformers_brawler.exe
```

Back up `save_data.dat` before testing. Record any compiler output or the exact level/objective if the report still fails to appear.
