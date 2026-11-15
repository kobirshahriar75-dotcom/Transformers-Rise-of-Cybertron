#pragma once

#include "raylib.h"
#include "../core/SaveSystem.hpp"

class MissionCompletionUI {
public:
    enum class Action {
        NONE,
        NEXT_LEVEL,
        LEVEL_SELECT,
        REPLAY_LEVEL,
        MAIN_MENU
    };

    MissionCompletionUI();

    Action Update(const SaveData& saveData, int completedLevel);
    void ResetSelection() { m_selectedItem = 0; }
    void Draw(const SaveData& saveData, int completedLevel, int score, int kills,
              int coresEarned, float completionTime) const;

private:
    int m_selectedItem;
};
