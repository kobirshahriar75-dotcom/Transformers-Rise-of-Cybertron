# Level 7 Aerial Combat Regression Tests

These checks cover the Level 7 flight/combat integration and guard Level 8's existing difficulty profile.

## Automated source-level checks

Run from the project root:

```bash
python tools/verify_aerial_balance.py
```

The script verifies:
- Jetpack Mode is enabled for Levels 7 and 8.
- Level 7 starts at a safe aerial altitude; Level 8 retains its separate spawn altitude.
- Level 7 seeker waves use 3, 4, 5, and 6 enemies (18 total).
- Level 8 retains 4, 5, 6, and 7 seekers (22 total).
- Level 7 seekers use 10 damage, 3.0-second attack intervals, and 520 projectile speed.
- Level 8 seekers retain 14 damage, 2.2-second intervals, and 650 projectile speed.
- Jetpack primary attacks fire player plasma bolts instead of ground melee.
- Jetpack special attacks use the aerial two-bolt overload and spend 30 energy.
- Ground melee hitboxes are disabled in Jetpack Mode.
- Soundwave's Level 6 ranged salvo remains free of an unintended melee hitbox.

## In-game acceptance tests

1. Start Level 7. Confirm Optimus begins airborne with the Jetpack HUD prompt.
2. Hold W/Up/Space and S/Down to verify vertical flight; use A/D to strafe.
3. Press J/Z. Confirm a cyan plasma bolt fires horizontally and can damage a Seeker.
4. Press K/X with at least 30 energy. Confirm the two-bolt overload fires and energy decreases by 30.
5. Confirm no sword melee hitbox or ground-only combo is triggered in aerial mode.
6. Clear the four waves. Confirm the wave counts are 3, 4, 5, and 6.
7. Verify Level 7 seeker shots are visibly dodgeable and deal 10 damage on a confirmed hit.
8. Complete Level 7 and proceed to Level 8. Confirm Jetpack Mode remains available and the original higher seeker pressure is retained.
9. Confirm Starscream remains the Level 8 boss and the existing boss fight is unchanged by Level 7 wave tuning.
10. Replay Level 6 and confirm the Soundwave balance corrections remain in place.

A full gameplay acceptance run must be performed on the user's Windows/MSYS2 UCRT64 installation.
