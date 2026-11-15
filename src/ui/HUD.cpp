#include "HUD.hpp"
#include "../core/Constants.hpp"
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdio>

HUD::HUD() {
}

void HUD::DrawPlayerStatus(const Player& player) const {
    // Backdrop Panel (Top-Left)
    float panelX = 24.0f;
    float panelY = 20.0f;
    float panelW = 340.0f;
    float panelH = 92.0f;

    DrawRectangleRounded({ panelX, panelY, panelW, panelH }, 0.15f, 4, Constants::HUD_BG);
    DrawRectangleRoundedLines({ panelX, panelY, panelW, panelH }, 0.15f, 4, Constants::HUD_BORDER);
    DrawRectangle((int)panelX + 10, (int)panelY + 4, 72, 2, Constants::AUTOBOT_RED);
    DrawRectangle((int)panelX + 86, (int)panelY + 4, 238, 2,
        ColorAlpha(Constants::ENERGON_CYAN, 0.45f + 0.25f * std::sin((float)GetTime() * 2.0f)));

    // Optimus Prime Stylized Face Portrait Icon
    float portX = panelX + 10.0f;
    float portY = panelY + 10.0f;
    float portSize = 72.0f;

    DrawRectangleRounded({ portX, portY, portSize, portSize }, 0.2f, 4, { 25, 35, 55, 255 });
    DrawRectangleRoundedLines({ portX, portY, portSize, portSize }, 0.2f, 4, Constants::AUTOBOT_BLUE);

    // Mini helmet representation
    DrawRectangle((int)(portX + 16), (int)(portY + 14), 40, 36, Constants::AUTOBOT_BLUE);
    DrawRectangle((int)(portX + 12), (int)(portY + 10), 6, 28, Constants::AUTOBOT_SILVER);
    DrawRectangle((int)(portX + 54), (int)(portY + 10), 6, 28, Constants::AUTOBOT_SILVER);
    // Silver faceplate
    DrawRectangle((int)(portX + 22), (int)(portY + 34), 28, 16, Constants::AUTOBOT_SILVER);
    // Glowing cyan optics
    DrawRectangle((int)(portX + 24), (int)(portY + 24), 24, 6, Constants::ENERGON_CYAN);

    // HP Bar
    float barX = panelX + 96.0f;
    float barY = panelY + 18.0f;
    float barW = 224.0f;
    float barH = 18.0f;

    float hpPct = player.GetHP() / player.GetMaxHP();
    Color hpColor = (hpPct > 0.5f) ? Color{ 40, 220, 100, 255 } : ((hpPct > 0.25f) ? Color{ 240, 180, 20, 255 } : Color{ 240, 40, 40, 255 });

    DrawRectangle((int)barX, (int)barY, (int)barW, (int)barH, { 35, 40, 50, 255 });
    DrawRectangle((int)barX, (int)barY, (int)(barW * hpPct), (int)barH, hpColor);
    DrawRectangleLines((int)barX, (int)barY, (int)barW, (int)barH, Constants::HUD_BORDER);

    std::string hpStr = "HP " + std::to_string((int)player.GetHP()) + " / " + std::to_string((int)player.GetMaxHP());
    DrawText(hpStr.c_str(), (int)barX + 6, (int)barY + 2, 14, RAYWHITE);

    // Energy / Energon Meter
    float eBarY = barY + 24.0f;
    float eBarH = 12.0f;
    float ePct = player.GetEnergy() / player.GetMaxEnergy();

    DrawRectangle((int)barX, (int)eBarY, (int)barW, (int)eBarH, { 35, 40, 50, 255 });
    DrawRectangle((int)barX, (int)eBarY, (int)(barW * ePct), (int)eBarH, Constants::ENERGON_CYAN);
    DrawRectangleLines((int)barX, (int)eBarY, (int)barW, (int)eBarH, Constants::HUD_BORDER);
    if (ePct > 0.02f) {
        const float shimmerX = barX + std::fmod((float)GetTime() * 95.0f, std::max(1.0f, barW * ePct));
        DrawLine((int)shimmerX, (int)eBarY + 1, (int)shimmerX, (int)(eBarY + eBarH - 1),
                 { 220, 255, 255, 150 });
    }

    std::string eStr = "ENERGON " + std::to_string((int)player.GetEnergy()) + "%";
    DrawText(eStr.c_str(), (int)barX + 6, (int)eBarY - 1, 11, RAYWHITE);

    // Active Mode Indicator Badge
    float badgeY = eBarY + 18.0f;
    PlayerMode mode = player.GetMode();
    const char* modeName = "ENERGON BLADE";
    Color modeColor = Constants::ENERGON_ORANGE;

    if (player.IsJetpackMode()) {
        modeName = "JETPACK FLIGHT";
        modeColor = Constants::ENERGON_CYAN;
    } else if (mode == PlayerMode::PLASMA_CANNON) {
        modeName = "PLASMA CANNON";
        modeColor = Constants::ENERGON_CYAN;
    } else if (mode == PlayerMode::TRUCK) {
        modeName = "TRUCK MODE";
        modeColor = Constants::AUTOBOT_RED;
    }

    DrawRectangleRounded({ barX, badgeY, 140.0f, 16.0f }, 0.4f, 4, modeColor);
    DrawText(modeName, (int)barX + 8, (int)badgeY + 2, 11, BLACK);

    // Combo Indicator (if in Blade mode and not jetpack)
    if (!player.IsJetpackMode() && mode == PlayerMode::ENERGON_BLADE && player.GetComboStep() > 0) {
        std::string comboStr = "COMBO " + std::to_string(player.GetComboStep()) + "/3";
        DrawText(comboStr.c_str(), (int)barX + 155, (int)badgeY + 2, 12, Constants::ENERGON_CYAN);
    }
}

void HUD::DrawBossHealthBar(const Boss& boss) const {
    float barW = 560.0f;
    float barH = 24.0f;
    float barX = ((float)Constants::SCREEN_WIDTH - barW) * 0.5f;
    float barY = 24.0f;

    // Per-boss name, phase label, colors and max HP
    const char* bossName = "MEGATRON";
    const char* bossPhase1Label = "DECEPTICON WARLORD";
    const char* bossPhase2Label = "PHASE 2 - OVERDRIVE";
    Color bossHpCol1 = { 180, 25, 45, 255 };
    Color bossHpCol2 = { 220, 40, 240, 255 };

    LevelBossType bType = boss.GetBossType();
    if (bType == LevelBossType::SHOCKWAVE) {
        bossName          = "SHOCKWAVE";
        bossPhase1Label   = "DECEPTICON CHIEF SCIENTIST";
        bossPhase2Label   = "PHASE 2 - HYPER LOGIC STORM";
        bossHpCol1 = { 160, 35, 220, 255 };
        bossHpCol2 = { 230, 50, 255, 255 };
    } else if (bType == LevelBossType::SOUNDWAVE) {
        bossName          = "SOUNDWAVE";
        bossPhase1Label   = "DECEPTICON COMMUNICATIONS OFFICER";
        bossPhase2Label   = "PHASE 2 - SONIC OVERDRIVE";
        bossHpCol1 = { 25, 90, 210, 255 };
        bossHpCol2 = { 60, 200, 255, 255 };
    } else if (bType == LevelBossType::STARSCREAM) {
        bossName          = "STARSCREAM";
        bossPhase1Label   = "DECEPTICON AIR COMMANDER";
        bossPhase2Label   = "PHASE 2 - SUPERSONIC RAMPAGE";
        bossHpCol1 = { 200, 25, 30, 255 };
        bossHpCol2 = { 255, 80, 20, 255 };
    }

    bool phaseTwo = boss.IsInPhaseTwo();
    const char* bossTitle;
    static char bossTitleBuf[128];
    if (phaseTwo) {
        snprintf(bossTitleBuf, sizeof(bossTitleBuf), "%s [%s]", bossName, bossPhase2Label);
    } else {
        snprintf(bossTitleBuf, sizeof(bossTitleBuf), "%s - %s", bossName, bossPhase1Label);
    }
    bossTitle = bossTitleBuf;

    Color titleColor = phaseTwo ? Constants::ENERGON_PURPLE : RAYWHITE;
    if (bType == LevelBossType::SOUNDWAVE && phaseTwo) titleColor = Constants::ENERGON_CYAN;
    if (bType == LevelBossType::STARSCREAM && phaseTwo) titleColor = Constants::AUTOBOT_RED;

    int textW = MeasureText(bossTitle, 17);
    DrawText(bossTitle, (int)(((float)Constants::SCREEN_WIDTH - textW) * 0.5f), (int)barY - 20, 17, titleColor);

    // Outer Background Box
    DrawRectangle((int)barX - 4, (int)barY - 2, (int)barW + 8, (int)barH + 4, Constants::HUD_BG);
    DrawRectangle((int)barX, (int)barY, (int)barW, (int)barH, { 45, 30, 55, 255 });

    // Boss HP fill
    float pct = boss.GetHP() / boss.GetMaxHP();
    Color bossHpCol = phaseTwo ? bossHpCol2 : bossHpCol1;
    DrawRectangle((int)barX, (int)barY, (int)(barW * pct), (int)barH, bossHpCol);
    DrawRectangleLines((int)barX - 4, (int)barY - 2, (int)barW + 8, (int)barH + 4, Constants::DECEPTICON_PURPLE);

    // Segmentation ticks at 25% intervals
    for (int t = 1; t < 4; t++) {
        float tx = barX + (barW * 0.25f * t);
        DrawLine((int)tx, (int)barY, (int)tx, (int)(barY + barH), BLACK);
    }

    // HP text: show actual HP / actual max HP
    std::string bossHpText = std::to_string((int)boss.GetHP()) + " / " + std::to_string((int)boss.GetMaxHP());
    int hpTextW = MeasureText(bossHpText.c_str(), 14);
    DrawText(bossHpText.c_str(), (int)(((float)Constants::SCREEN_WIDTH - hpTextW) * 0.5f), (int)barY + 5, 14, RAYWHITE);
}

void HUD::DrawWaveInfo(const Arena& arena) const {
    // Top-Right Mission Status Panel
    float panelW = 230.0f;
    float panelH = 85.0f;
    float panelX = (float)Constants::SCREEN_WIDTH - panelW - 24.0f;
    float panelY = 20.0f;

    DrawRectangleRounded({ panelX, panelY, panelW, panelH }, 0.15f, 4, Constants::HUD_BG);
    DrawRectangleRoundedLines({ panelX, panelY, panelW, panelH }, 0.15f, 4, Constants::HUD_BORDER);
    DrawRectangle((int)panelX + 10, (int)panelY + 4, 54, 2, Constants::ENERGON_CYAN);

    // Sector Name
    const LevelInfo& lvl = arena.GetLevelInfo();
    std::string sectorStr = "SEC " + std::to_string(lvl.number) + ": " + lvl.name;
    DrawText(sectorStr.c_str(), (int)panelX + 14, (int)panelY + 10, 13, Constants::AUTOBOT_SILVER);

    // Wave number
    bool isBossWave = (arena.GetCurrentWave() == arena.GetTotalWaves() && lvl.bossType != LevelBossType::NONE);
    std::string waveStr = isBossWave ? "BOSS ENCOUNTER" : ("WAVE " + std::to_string(arena.GetCurrentWave()) + " / " + std::to_string(arena.GetTotalWaves()));
    Color waveColor = isBossWave ? Constants::ENERGON_PURPLE : Constants::ENERGON_CYAN;
    DrawText(waveStr.c_str(), (int)panelX + 14, (int)panelY + 28, 16, waveColor);

    // Score & Kills
    std::string scoreStr = "SCORE: " + std::to_string(arena.GetScore());
    std::string killsStr = "KILLS: " + std::to_string(arena.GetKills());
    DrawText(scoreStr.c_str(), (int)panelX + 14, (int)panelY + 48, 13, RAYWHITE);
    DrawText(killsStr.c_str(), (int)panelX + 130, (int)panelY + 48, 13, { 255, 215, 60, 255 });


    // Wave Banner Notification (if active)
    if (arena.GetBannerTimer() > 0.0f) {
        float alpha = std::min(1.0f, arena.GetBannerTimer());
        std::string banner = arena.GetWaveBanner();
        int bWidth = MeasureText(banner.c_str(), 24);
        float bX = ((float)Constants::SCREEN_WIDTH - bWidth) * 0.5f;
        float bY = 140.0f;

        Color bBg = { 10, 15, 25, (unsigned char)(210 * alpha) };
        DrawRectangle((int)bX - 25, (int)bY - 8, bWidth + 50, 42, bBg);
        DrawRectangleLines((int)bX - 25, (int)bY - 8, bWidth + 50, 42, { 0, 235, 255, (unsigned char)(220 * alpha) });
        DrawText(banner.c_str(), (int)bX, (int)bY, 24, { 255, 255, 255, (unsigned char)(255 * alpha) });
    }
}

void HUD::DrawControlsPrompt(const Player& player) const {
    // Bottom Controls Bar
    float barY = (float)Constants::SCREEN_HEIGHT - 32.0f;
    DrawRectangle(0, (int)barY, Constants::SCREEN_WIDTH, 32, { 12, 16, 24, 210 });
    DrawLine(0, (int)barY, Constants::SCREEN_WIDTH, (int)barY, { 45, 60, 85, 255 });

    // Show aerial controls in jetpack/aerial mode
    const char* controls;
    if (player.IsJetpackMode()) {
        controls = "[A/D] Strafe   [W/S] Fly Up/Down   [SPACE] Ascend   [HOLD J/Z] Continuous Plasma   [K/X] Overload   [P/ESC] Pause   [JETPACK MODE]";
    } else {
        controls = "[A/D] Move   [W/SPACE] Jump   [J/Z] Attack   [K/X] Special/Shoot   [C or 1-2-3] Transform   [H] Horn   [P/ESC] Pause";
    }
    int textW = MeasureText(controls, 13);
    int textX = (Constants::SCREEN_WIDTH - textW) / 2;
    DrawText(controls, textX, (int)barY + 9, 13, Constants::AUTOBOT_SILVER);
}

void HUD::Draw(const Player& player, const Arena& arena, const Boss* boss) const {
    DrawPlayerStatus(player);
    DrawWaveInfo(arena);

    if (boss && !boss->IsDead()) {
        DrawBossHealthBar(*boss);
    }

    DrawControlsPrompt(player);
}
