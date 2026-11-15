# Campaign Mode Configuration Regression

## Official campaign rules

- Levels 1-6 and 9-10: exactly three selectable modes (Energon Blade, Plasma Cannon, Truck).
- Every level starts with Energon Blade selected because `Player::Reset` restores `PlayerMode::ENERGON_BLADE` and `Game::StartLevel` resets the player before applying the level-specific aerial setting.
- Levels 7-8: Jetpack aerial configuration is the only available mode. Numeric transforms and cycle-transform input are disabled, and the public transform/cycle methods also reject changes while Jetpack mode is active.
- Levels 7-8: holding J or Z fires plasma bolts continuously at the existing 0.18-second cooldown cadence. K/X remains the energy-consuming overload special.

## Manual acceptance checks

1. Start each of Levels 1-6 and 9-10. Confirm Energon Blade is active at spawn.
2. In each of those levels, press 1, 2, and 3; confirm Energon Blade, Plasma Cannon, and Truck are selectable. Confirm C/Left Shift cycles all three.
3. Complete/replay a level after using another mode. Confirm the next level/replay starts in Energon Blade.
4. Start Level 7. Confirm Jetpack is active, flight controls work, and 1/2/3/C/Left Shift cannot change mode.
5. Hold J or Z in Level 7. Confirm repeated plasma bolts are fired without repeated key presses.
6. Repeat checks 4-5 in Level 8.
7. Confirm Level 8 still retains its existing boss encounter and Levels 1-6 and 9-10 retain their existing ground-mode behavior.
