#pragma once

#include "raylib.h"
#include <cstdint>

namespace Constants {

    // Display & Engine Settings
    constexpr int SCREEN_WIDTH = 1280;
    constexpr int SCREEN_HEIGHT = 720;
    constexpr int TARGET_FPS = 60;
    constexpr const char* WINDOW_TITLE = "Transformers: Cybertron Brawler (30-Day Vertical Slice)";

    // Arena World Dimensions
    constexpr float ARENA_WIDTH = 3600.0f;
    constexpr float ARENA_MIN_X = 50.0f;
    constexpr float ARENA_MAX_X = ARENA_WIDTH - 50.0f;
    constexpr float GROUND_Y = 560.0f;
    constexpr float GRAVITY = 1600.0f;

    // Player (Optimus Prime) Configuration
    constexpr float PLAYER_MAX_HP = 100.0f;
    constexpr float PLAYER_MAX_ENERGY = 100.0f;
    constexpr float PLAYER_ENERGY_REGEN_RATE = 10.0f; // energy per second

    // Robot Mode Physics
    constexpr float ROBOT_MOVE_SPEED = 360.0f;
    constexpr float ROBOT_ACCEL = 2200.0f;
    constexpr float ROBOT_DECEL = 2400.0f;
    constexpr float ROBOT_JUMP_FORCE = -680.0f;
    constexpr float ROBOT_WIDTH = 54.0f;
    constexpr float ROBOT_HEIGHT = 92.0f;

    // Truck Mode Physics
    constexpr float TRUCK_MAX_SPEED = 820.0f;
    constexpr float TRUCK_ACCEL = 1400.0f;
    constexpr float TRUCK_DECEL = 1000.0f;
    constexpr float TRUCK_RAM_DAMAGE = 35.0f;
    constexpr float TRUCK_WIDTH = 110.0f;
    constexpr float TRUCK_HEIGHT = 52.0f;

    // Combat Timing & Feel
    constexpr float HIT_STOP_LIGHT = 0.045f;
    constexpr float HIT_STOP_HEAVY = 0.085f;
    constexpr float HIT_STOP_CRIT = 0.12f;

    constexpr float SHAKE_LIGHT = 4.0f;
    constexpr float SHAKE_MEDIUM = 8.0f;
    constexpr float SHAKE_HEAVY = 15.0f;

    // Colors - Autobot & Cybertron Palette
    inline const Color AUTOBOT_RED = { 220, 30, 40, 255 };
    inline const Color AUTOBOT_BLUE = { 30, 80, 210, 255 };
    inline const Color AUTOBOT_SILVER = { 200, 210, 225, 255 };
    inline const Color ENERGON_CYAN = { 0, 235, 255, 255 };
    inline const Color ENERGON_ORANGE = { 255, 140, 20, 255 };
    inline const Color ENERGON_PURPLE = { 200, 40, 240, 255 };
    inline const Color DECEPTICON_PURPLE = { 150, 40, 220, 255 };
    inline const Color DECEPTICON_DARK = { 45, 40, 60, 255 };
    inline const Color HUD_BG = { 18, 22, 32, 230 };
    inline const Color HUD_BORDER = { 70, 95, 130, 255 };

} // namespace Constants

enum class PlayerMode {
    ENERGON_BLADE = 0,
    PLASMA_CANNON = 1,
    TRUCK = 2
};

enum class GameState {
    OPENING_CINEMATIC,
    MENU,
    LEVEL_SELECT,
    SETTINGS,
    PLAYING,
    PAUSED,
    GAME_OVER,
    VICTORY,
    CREDITS
};

enum class Faction {
    PLAYER,
    ENEMY
};

enum class EnemyType {
    DRONE,
    BRUTE,
    SEEKER_AERIAL,
    BOSS_MEGATRON
};
