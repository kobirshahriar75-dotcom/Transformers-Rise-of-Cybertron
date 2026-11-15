# Level 6 and Level 7 Balance Refinement Report

## Scope

This integrated update retains the existing keyboard-only/UI refinement and the previous Soundwave balance correction, then fixes the Level 7 aerial-combat progression. It does not redesign the campaign or alter the Level 8 seeker count/damage/cadence tuning.

## Level 6 — Soundwave

The project baseline already includes the Soundwave changes:
- Single sonic disc damage: 18.
- Shoulder salvo disc damage: 16 each.
- Attack telegraph: 0.70 seconds in Phase 1 and 0.50 seconds in Phase 2.
- Recovery: 1.90 seconds in Phase 1 and 1.35 seconds in Phase 2.
- Soundwave's ranged shoulder salvo does not create the shared close-range melee hitbox.

## Level 7 — Aerial Battlefield

### Player access and combat
- Jetpack Mode is enabled from the start of Level 7.
- Level 7 starts at Y=380, leaving room to manoeuvre upward and downward.
- Level 8 retains its own aerial spawn altitude (Y=300) and Starscream encounter.
- Fixed the attack-routing defect: when Jetpack Mode is active, J/Z fires the aerial plasma bolt and K/X fires the aerial overload, even though the underlying player mode remains Energon Blade.
- Ground melee hitboxes are explicitly suppressed while Jetpack Mode is active.

### Level 7 seeker balance
- Wave counts: 3, 4, 5, 6 (18 total, previously 4, 5, 6, 7 / 22 total).
- Projectile damage: 10 (previously 14).
- Firing interval: 3.0 seconds (previously 2.2 seconds).
- Projectile speed: 520 (previously 650).

Level 8 retains the original 4, 5, 6, 7 seeker progression and its original 14 damage / 2.2 second / 650 speed profile.

## Verification status

- Source-level regression script added: `tools/verify_aerial_balance.py`.
- In-game acceptance checklist added: `tests/LEVEL_7_AERIAL_COMBAT_REGRESSION.md`.
- A real Raylib/MinGW build and gameplay run are not claimed from this environment; they must be performed on the Windows/MSYS2 UCRT64 setup.
