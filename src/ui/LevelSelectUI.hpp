#pragma once

#include "raylib.h"
#include "../core/SaveSystem.hpp"
#include "../world/LevelData.hpp"

class LevelSelectUI {
public:
    LevelSelectUI();

    // Returns true when player selects an unlocked level to launch
    bool Update(const SaveData& saveData, int& outSelectedLevel, bool& requestBackToMenu);
    void Draw(const SaveData& saveData) const;

    int GetSelectedLevel() const { return m_selectedLevel; }
    void SetSelectedLevel(int level) { m_selectedLevel = level; m_backSelected = false; }

private:
    int m_selectedLevel; // 1 to 10
    bool m_backSelected;
    float m_animTime;

    void DrawLevelCard(const LevelInfo& level, const SaveData& saveData, Rectangle bounds, bool isSelected) const;
    void DrawMissionBriefing(const LevelInfo& level, const SaveData& saveData, Rectangle bounds, bool launchFocused) const;
};
