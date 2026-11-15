#include "MissionCompletionUI.hpp"
#include "../core/Constants.hpp"
#include "../world/LevelData.hpp"
#include <algorithm>
#include <cmath>
#include <string>

namespace {
    constexpr float PANEL_W = 760.0f;
    constexpr float PANEL_H = 560.0f;
    constexpr float BUTTON_H = 42.0f;
    constexpr float BUTTON_GAP = 10.0f;

    Rectangle PanelRect() {
        return { (Constants::SCREEN_WIDTH - PANEL_W) * 0.5f,
                 (Constants::SCREEN_HEIGHT - PANEL_H) * 0.5f,
                 PANEL_W, PANEL_H };
    }

    Rectangle ButtonRect(int index) {
        const Rectangle panel = PanelRect();
        const float y = panel.y + 350.0f + index * (BUTTON_H + BUTTON_GAP);
        return { panel.x + 70.0f, y, PANEL_W - 140.0f, BUTTON_H };
    }

    std::string FormatTime(float seconds) {
        const int total = std::max(0, static_cast<int>(seconds));
        const int minutes = total / 60;
        const int remainder = total % 60;
        return std::to_string(minutes) + "m " + (remainder < 10 ? "0" : "") +
               std::to_string(remainder) + "s";
    }
}

MissionCompletionUI::MissionCompletionUI()
    : m_selectedItem(0) {
}

MissionCompletionUI::Action MissionCompletionUI::Update(const SaveData& saveData, int completedLevel) {
    const bool hasNextLevel = completedLevel >= 1 && completedLevel < 10 &&
        SaveSystem::IsLevelUnlocked(saveData, completedLevel + 1);
    const int optionCount = hasNextLevel ? 4 : 3;
    m_selectedItem = std::clamp(m_selectedItem, 0, optionCount - 1);

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        m_selectedItem = (m_selectedItem - 1 + optionCount) % optionCount;
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        m_selectedItem = (m_selectedItem + 1) % optionCount;

    // Only deliberate UI confirmation keys activate results. J remains a combat key.
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (hasNextLevel) {
            if (m_selectedItem == 0) return Action::NEXT_LEVEL;
            if (m_selectedItem == 1) return Action::LEVEL_SELECT;
            if (m_selectedItem == 2) return Action::REPLAY_LEVEL;
            if (m_selectedItem == 3) return Action::MAIN_MENU;
        } else {
            if (m_selectedItem == 0) return Action::LEVEL_SELECT;
            if (m_selectedItem == 1) return Action::REPLAY_LEVEL;
            if (m_selectedItem == 2) return Action::MAIN_MENU;
        }
    }
    return Action::NONE;
}

void MissionCompletionUI::Draw(const SaveData& saveData, int completedLevel, int score,
                               int kills, int coresEarned, float completionTime) const {
    const Rectangle panel = PanelRect();
    const bool hasNextLevel = completedLevel >= 1 && completedLevel < 10 &&
        SaveSystem::IsLevelUnlocked(saveData, completedLevel + 1);
    const LevelInfo& info = LevelDatabase::GetLevel(completedLevel);

    DrawRectangle(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, { 2, 8, 18, 225 });
    DrawRectangleGradientV(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT,
                            { 5, 18, 35, 100 }, { 5, 8, 20, 210 });
    DrawRectangleRounded(panel, 0.08f, 8, { 13, 23, 39, 248 });
    DrawRectangleRoundedLinesEx(panel, 0.08f, 8, 2.5f, Constants::ENERGON_CYAN);
    const float scanY = panel.y + 145.0f + std::fmod(GetTime() * 38.0, 330.0);
    DrawLine((int)panel.x + 24, (int)scanY, (int)(panel.x + panel.width - 24), (int)scanY,
             { 0, 210, 255, 32 });
    DrawRectangle((int)panel.x + 24, (int)panel.y + 14, 5, 34, Constants::AUTOBOT_RED);
    DrawRectangle((int)(panel.x + panel.width - 29), (int)panel.y + 14, 5, 34, Constants::ENERGON_CYAN);

    const char* title = completedLevel == 10 ? "CAMPAIGN COMPLETE" : "MISSION COMPLETE";
    int titleWidth = MeasureText(title, 34);
    DrawText(title, static_cast<int>(panel.x + (panel.width - titleWidth) * 0.5f),
             static_cast<int>(panel.y + 24), 34, Constants::ENERGON_CYAN);

    const std::string levelLine = "LEVEL " + std::to_string(completedLevel) + "  //  " + info.name;
    int levelWidth = MeasureText(levelLine.c_str(), 21);
    DrawText(levelLine.c_str(), static_cast<int>(panel.x + (panel.width - levelWidth) * 0.5f),
             static_cast<int>(panel.y + 75), 21, RAYWHITE);

    const std::string subtitle = info.subtitle;
    int subtitleWidth = MeasureText(subtitle.c_str(), 15);
    DrawText(subtitle.c_str(), static_cast<int>(panel.x + (panel.width - subtitleWidth) * 0.5f),
             static_cast<int>(panel.y + 108), 15, Constants::AUTOBOT_SILVER);

    DrawLine(static_cast<int>(panel.x + 55), static_cast<int>(panel.y + 145),
             static_cast<int>(panel.x + panel.width - 55), static_cast<int>(panel.y + 145),
             { 70, 110, 140, 180 });

    const std::string enemies = "ENEMIES ELIMINATED: " + std::to_string(kills);
    const std::string combatScore = "COMBAT SCORE: " + std::to_string(score);
    const std::string time = "COMPLETION TIME: " + FormatTime(completionTime);
    const std::string cores = "ENERGON CORES EARNED: +" + std::to_string(coresEarned);
    const std::string best = "LEVEL BEST: " + std::to_string(saveData.levelHighScores[completedLevel - 1]);

    DrawText(enemies.c_str(), static_cast<int>(panel.x + 90), static_cast<int>(panel.y + 170), 19, RAYWHITE);
    DrawText(combatScore.c_str(), static_cast<int>(panel.x + 90), static_cast<int>(panel.y + 205), 19, { 255, 215, 90, 255 });
    DrawText(time.c_str(), static_cast<int>(panel.x + 410), static_cast<int>(panel.y + 170), 19, RAYWHITE);
    DrawText(cores.c_str(), static_cast<int>(panel.x + 410), static_cast<int>(panel.y + 205), 19, { 120, 245, 170, 255 });
    DrawText(best.c_str(), static_cast<int>(panel.x + 90), static_cast<int>(panel.y + 245), 17, Constants::AUTOBOT_SILVER);

    std::string progression;
    if (hasNextLevel) {
        progression = "UNLOCKED: LEVEL " + std::to_string(completedLevel + 1) + " - " +
                      LevelDatabase::GetLevel(completedLevel + 1).name;
    } else if (completedLevel == 10) {
        progression = "ALL TEN CAMPAIGN LEVELS COMPLETED";
    } else {
        progression = "CAMPAIGN PROGRESS SAVED";
    }
    int progressWidth = MeasureText(progression.c_str(), 16);
    DrawText(progression.c_str(), static_cast<int>(panel.x + (panel.width - progressWidth) * 0.5f),
             static_cast<int>(panel.y + 285), 16, { 120, 245, 170, 255 });

    const char* optionsWithNext[] = {
        "NEXT UNLOCKED LEVEL",
        "CAMPAIGN MAP / LEVEL SELECT",
        "REPLAY THIS LEVEL",
        "RETURN TO MAIN MENU"
    };
    const char* optionsFinal[] = {
        "CAMPAIGN MAP / LEVEL SELECT",
        "REPLAY THIS LEVEL",
        "RETURN TO MAIN MENU"
    };
    const int optionCount = hasNextLevel ? 4 : 3;
    for (int i = 0; i < optionCount; ++i) {
        const Rectangle button = ButtonRect(i);
        const bool selected = i == m_selectedItem;
        DrawRectangleRounded(button, 0.18f, 6,
            selected ? Color{ 25, 65, 88, 245 } : Color{ 24, 36, 53, 235 });
        DrawRectangleRoundedLinesEx(button, 0.18f, 6, selected ? 2.0f : 1.0f,
            selected ? Constants::ENERGON_CYAN : Constants::HUD_BORDER);
        const char* label = hasNextLevel ? optionsWithNext[i] : optionsFinal[i];
        const int labelWidth = MeasureText(label, 16);
        DrawText(label, static_cast<int>(button.x + (button.width - labelWidth) * 0.5f),
                 static_cast<int>(button.y + 12), 16, selected ? RAYWHITE : LIGHTGRAY);
    }

    const char* hint = "[UP / DOWN] SELECT     [ENTER / SPACE] CONFIRM";
    const int hintWidth = MeasureText(hint, 12);
    DrawText(hint, (Constants::SCREEN_WIDTH - hintWidth) / 2, Constants::SCREEN_HEIGHT - 24,
             12, Constants::AUTOBOT_SILVER);
}
