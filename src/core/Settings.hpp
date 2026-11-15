#pragma once

#include "raylib.h"

struct GameSettings {
    // Audio settings
    bool musicEnabled = true;
    bool sfxEnabled = true;
    float masterVolume = 1.0f; // 0.0f to 1.0f
    float musicVolume = 0.75f;
    float sfxVolume = 0.85f;

    // Visual settings
    int colorPalette = 0;      // 0: Cybertron Indigo, 1: High Contrast Neon, 2: Deep Space
    float brightness = 1.0f;   // 0.8f to 1.2f
    float contrast = 1.0f;     // 0.8f to 1.2f
    int screenWidth = 1280;
    int screenHeight = 720;
    bool fullscreen = false;
    bool vsync = true;
    float screenShakeIntensity = 1.0f; // 0.0f to 1.5f
    float visualEffectIntensity = 1.0f;

    // Gameplay settings
    bool subtitlesEnabled = true;
    int difficulty = 1;        // 0: Recruit, 1: Autobot Warrior (Normal), 2: Prime Veteran (Hard)
    bool damageNumbers = true;
    float cameraTrackingSpeed = 1.0f;
};
