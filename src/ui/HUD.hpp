#pragma once

#include "raylib.h"
#include "../entities/Player.hpp"
#include "../entities/Boss.hpp"
#include "../world/Arena.hpp"

class HUD {
public:
    HUD();

    void Draw(const Player& player, const Arena& arena, const Boss* boss) const;

private:
    void DrawPlayerStatus(const Player& player) const;
    void DrawBossHealthBar(const Boss& boss) const;
    void DrawWaveInfo(const Arena& arena) const;
    void DrawControlsPrompt(const Player& player) const;
};
