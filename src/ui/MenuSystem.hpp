#pragma once

#include "raylib.h"
#include "../core/Constants.hpp"
#include "../core/SaveSystem.hpp"

class MenuSystem {
public:
    MenuSystem();

    void Update(GameState& state, const SaveData& saveData, bool& requestRestart, bool& requestQuit, int& outStartLevel);
    void Draw(GameState state, const SaveData& saveData, int score, int kills, int wave);
    void ResetSelection() { m_selectedItem = 0; }

private:
    int m_selectedItem;
    GameState m_lastState;
    bool m_showControlsModal;
    bool m_showCreditsModal;
    float m_animTime;

    void DrawMainMenu(const SaveData& saveData) const;
    void DrawPauseMenu() const;
    void DrawGameOver(int score, int kills, int wave) const;
    void DrawVictory(int score, int kills) const;
    void DrawControlsModal() const;
    void DrawCreditsModal() const;
};
