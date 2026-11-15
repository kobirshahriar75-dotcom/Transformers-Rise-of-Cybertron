#pragma once

#include "raylib.h"
#include "../core/Settings.hpp"
#include "../core/SaveSystem.hpp"

class SettingsUI {
public:
    SettingsUI();

    // Updates navigation and adjustment of settings.
    // Returns true if player requests to return to Main Menu.
    bool Update(GameSettings& settings, SaveData& saveData, bool& requestSave);
    void Draw(const GameSettings& settings) const;

private:
    int m_selectedCategory; // 0: Audio, 1: Visuals, 2: Gameplay
    int m_selectedItem;
    float m_animTime;

    void DrawAudioTab(const GameSettings& settings, Rectangle area) const;
    void DrawVisualTab(const GameSettings& settings, Rectangle area) const;
    void DrawGameplayTab(const GameSettings& settings, Rectangle area) const;
};
