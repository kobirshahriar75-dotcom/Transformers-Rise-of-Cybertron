#include "LevelSelectUI.hpp"
#include "../core/Constants.hpp"
#include <cmath>
#include <string>

LevelSelectUI::LevelSelectUI()
    : m_selectedLevel(1)
    , m_backSelected(false)
    , m_animTime(0.0f)
{
}

bool LevelSelectUI::Update(const SaveData& saveData, int& outSelectedLevel, bool& requestBackToMenu) {
    m_animTime += GetFrameTime();

    if (IsKeyPressed(KEY_TAB)) m_backSelected = !m_backSelected;

    if (!m_backSelected) {
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            if ((m_selectedLevel - 1) % 5 > 0) --m_selectedLevel;
        }
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
            if ((m_selectedLevel - 1) % 5 < 4 && m_selectedLevel < 10) ++m_selectedLevel;
        }
        if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && m_selectedLevel > 5)
            m_selectedLevel -= 5;
        if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && m_selectedLevel <= 5)
            m_selectedLevel += 5;
    }

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        m_backSelected = false;
        requestBackToMenu = true;
        return false;
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (m_backSelected) {
            m_backSelected = false;
            requestBackToMenu = true;
            return false;
        }
        if (SaveSystem::IsLevelUnlocked(saveData, m_selectedLevel)) {
            outSelectedLevel = m_selectedLevel;
            return true;
        }
    }
    return false;
}

void LevelSelectUI::Draw(const SaveData& saveData) const {
    DrawRectangleGradientV(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT,
        { 10, 14, 24, 255 }, { 22, 28, 48, 255 });

    for (int y = 0; y < Constants::SCREEN_HEIGHT; y += 36) {
        DrawLine(0, y, Constants::SCREEN_WIDTH, y, { 30, 42, 68, 35 });
    }
    for (int x = 0; x < Constants::SCREEN_WIDTH; x += 64) {
        DrawLine(x, 0, x, Constants::SCREEN_HEIGHT, { 30, 42, 68, 25 });
    }
    const float scanY = std::fmod(m_animTime * 34.0f, (float)Constants::SCREEN_HEIGHT);
    DrawLine(0, (int)scanY, Constants::SCREEN_WIDTH, (int)scanY, { 0, 220, 255, 22 });
    DrawRectangle(32, 30, 4, 44, Constants::AUTOBOT_RED);
    DrawRectangle(38, 30, 2, 44, ColorAlpha(Constants::ENERGON_CYAN, 0.55f));

    const char* header = "CYBERTRON CAMPAIGN TACTICAL MAP";
    const char* subheader = "SELECT MISSION SECTOR -- ARROWS / WASD TO NAVIGATE";
    int hw = MeasureText(header, 32);
    int subw = MeasureText(subheader, 14);

    DrawText(header, (Constants::SCREEN_WIDTH - hw) / 2 + 2, 26, 32, { 180, 20, 30, 160 });
    DrawText(header, (Constants::SCREEN_WIDTH - hw) / 2, 24, 32, Constants::AUTOBOT_RED);
    DrawText(subheader, (Constants::SCREEN_WIDTH - subw) / 2, 64, 14, Constants::ENERGON_CYAN);

    float startX = 60.0f;
    float startY = 96.0f;
    float cardW = 216.0f;
    float cardH = 140.0f;
    float spacingX = 20.0f;
    float spacingY = 16.0f;

    for (int i = 1; i <= 10; ++i) {
        int col = (i - 1) % 5;
        int row = (i - 1) / 5;
        float x = startX + col * (cardW + spacingX);
        float y = startY + row * (cardH + spacingY);

        const LevelInfo& info = LevelDatabase::GetLevel(i);
        bool isSelected = (!m_backSelected && i == m_selectedLevel);
        DrawLevelCard(info, saveData, { x, y, cardW, cardH }, isSelected);
    }

    float briefY = startY + 2 * (cardH + spacingY) + 10.0f;
    float briefH = Constants::SCREEN_HEIGHT - briefY - 50.0f;
    DrawMissionBriefing(LevelDatabase::GetLevel(m_selectedLevel), saveData,
        { startX, briefY, Constants::SCREEN_WIDTH - startX * 2.0f, briefH }, !m_backSelected);

    // Bottom Navigation Bar
    DrawRectangle(0, Constants::SCREEN_HEIGHT - 38, Constants::SCREEN_WIDTH, 38, { 12, 16, 26, 240 });
    DrawLine(0, Constants::SCREEN_HEIGHT - 38, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT - 38, Constants::HUD_BORDER);

    const char* navText = "[ENTER / SPACE] DEPLOY    [ARROWS / WASD] NAVIGATE    [TAB] FOCUS BACK    [ESC] BACK";
    DrawText(navText, 40, Constants::SCREEN_HEIGHT - 26, 14, RAYWHITE);

    // Keyboard-focusable Back control (Tab selects it; Enter activates it).
    Rectangle backBtnRect = { (float)Constants::SCREEN_WIDTH - 180.0f, (float)Constants::SCREEN_HEIGHT - 34.0f, 150.0f, 28.0f };
    DrawRectangleRounded(backBtnRect, 0.2f, 4, m_backSelected ? Color{ 75, 28, 38, 245 } : Color{ 35, 18, 22, 220 });
    DrawRectangleRoundedLinesEx(backBtnRect, 0.2f, 4, m_backSelected ? 2.5f : 1.5f,
        m_backSelected ? Constants::ENERGON_CYAN : Constants::AUTOBOT_RED);
    DrawText("< MAIN MENU [ENTER]", (int)backBtnRect.x + 8, (int)backBtnRect.y + 7, 12, RAYWHITE);
}

void LevelSelectUI::DrawLevelCard(const LevelInfo& level, const SaveData& saveData, Rectangle bounds, bool isSelected) const {
    bool isUnlocked = SaveSystem::IsLevelUnlocked(saveData, level.number);
    bool isCompleted = saveData.completedLevels[level.number - 1];

    Color bgColor = isUnlocked ? Color{ 22, 30, 46, 230 } : Color{ 16, 18, 24, 230 };
    Color borderColor = Constants::HUD_BORDER;

    if (isSelected) {
        float pulse = (sinf(m_animTime * 6.0f) + 1.0f) * 0.5f;
        borderColor = isUnlocked ? ColorAlpha(Constants::ENERGON_CYAN, 0.7f + 0.3f * pulse)
                                 : ColorAlpha(Constants::AUTOBOT_RED, 0.7f + 0.3f * pulse);
    }

    DrawRectangleRounded(bounds, 0.12f, 4, bgColor);
    DrawRectangleRoundedLinesEx(bounds, 0.12f, 4, isSelected ? 3.0f : 1.5f, borderColor);

    std::string numStr = "SECTOR " + std::to_string(level.number);
    DrawRectangle((int)bounds.x + 8, (int)bounds.y + 8, 74, 18, { 12, 16, 24, 255 });
    DrawText(numStr.c_str(), (int)bounds.x + 12, (int)bounds.y + 11, 10, isUnlocked ? Constants::ENERGON_CYAN : GRAY);

    if (isCompleted) {
        DrawRectangle((int)(bounds.x + bounds.width - 80), (int)bounds.y + 8, 72, 18, { 25, 75, 40, 230 });
        DrawText("COMPLETED", (int)(bounds.x + bounds.width - 76), (int)bounds.y + 12, 9, { 120, 255, 140, 255 });
    } else if (isUnlocked) {
        DrawRectangle((int)(bounds.x + bounds.width - 70), (int)bounds.y + 8, 62, 18, { 18, 65, 95, 230 });
        DrawText("UNLOCKED", (int)(bounds.x + bounds.width - 66), (int)bounds.y + 12, 9, Constants::ENERGON_CYAN);
    } else {
        DrawRectangle((int)(bounds.x + bounds.width - 60), (int)bounds.y + 8, 52, 18, { 70, 25, 25, 230 });
        DrawText("LOCKED", (int)(bounds.x + bounds.width - 56), (int)bounds.y + 12, 9, { 255, 120, 120, 255 });
    }

    DrawText(level.name.c_str(), (int)bounds.x + 10, (int)bounds.y + 38, 14, isUnlocked ? RAYWHITE : GRAY);

    if (level.bossType != LevelBossType::NONE) {
        std::string bossName = "BOSS: ";
        if (level.bossType == LevelBossType::SHOCKWAVE) bossName += "SHOCKWAVE";
        else if (level.bossType == LevelBossType::SOUNDWAVE) bossName += "SOUNDWAVE";
        else if (level.bossType == LevelBossType::STARSCREAM) bossName += "STARSCREAM (AIR)";
        else if (level.bossType == LevelBossType::MEGATRON) bossName += "MEGATRON";

        DrawText(bossName.c_str(), (int)bounds.x + 10, (int)bounds.y + 60, 11, { 230, 80, 240, 255 });
    } else {
        std::string waveStr = std::to_string(level.totalWaves) + " COMBAT WAVES";
        DrawText(waveStr.c_str(), (int)bounds.x + 10, (int)bounds.y + 60, 11, LIGHTGRAY);
    }

    if (isCompleted) {
        std::string scoreStr = "HI: " + std::to_string(saveData.levelHighScores[level.number - 1]);
        DrawText(scoreStr.c_str(), (int)bounds.x + 10, (int)bounds.y + 82, 11, { 255, 215, 60, 255 });
    } else if (!isUnlocked) {
        std::string reqStr = "Req: Beat Sec " + std::to_string(level.number - 1);
        DrawText(reqStr.c_str(), (int)bounds.x + 10, (int)bounds.y + 82, 10, { 220, 100, 100, 255 });
    }

    if (isSelected) {
        DrawRectangle((int)bounds.x + 6, (int)(bounds.y + bounds.height - 6), (int)bounds.width - 12, 3,
            isUnlocked ? Constants::ENERGON_CYAN : Constants::AUTOBOT_RED);
    }
}

void LevelSelectUI::DrawMissionBriefing(const LevelInfo& level, const SaveData& saveData, Rectangle bounds, bool launchFocused) const {
    bool isUnlocked = SaveSystem::IsLevelUnlocked(saveData, level.number);
    bool isCompleted = saveData.completedLevels[level.number - 1];

    DrawRectangleRounded(bounds, 0.08f, 4, { 16, 22, 34, 240 });
    DrawRectangleRoundedLinesEx(bounds, 0.08f, 4, 1.5f, Constants::HUD_BORDER);

    Color stripeCol = isUnlocked ? Constants::ENERGON_CYAN : Constants::AUTOBOT_RED;
    DrawRectangle((int)bounds.x, (int)bounds.y, 6, (int)bounds.height, stripeCol);

    std::string fullTitle = "SECTOR " + std::to_string(level.number) + ": " + level.name + " (" + level.subtitle + ")";
    DrawText(fullTitle.c_str(), (int)bounds.x + 20, (int)bounds.y + 14, 18, isUnlocked ? RAYWHITE : LIGHTGRAY);

    DrawText("TACTICAL BRIEFING:", (int)bounds.x + 20, (int)bounds.y + 38, 12, Constants::AUTOBOT_SILVER);
    DrawText(level.description.c_str(), (int)bounds.x + 20, (int)bounds.y + 54, 14, isUnlocked ? RAYWHITE : GRAY);

    // Keyboard launch action: Enter / Space deploys the selected unlocked sector
    if (!isUnlocked) {
        std::string lockMsg = "[LOCKED] Sector " + std::to_string(level.number - 1) + " must be secured first!";
        DrawText(lockMsg.c_str(), (int)bounds.x + 20, (int)bounds.y + 78, 14, { 255, 90, 90, 255 });
    } else {
        Rectangle btnRec = { bounds.x + 20.0f, bounds.y + 74.0f, 320.0f, 32.0f };
        Color btnBg = launchFocused ? Color{ 35, 65, 95, 240 } : Color{ 18, 30, 44, 220 };
        Color btnBorder = launchFocused ? Constants::ENERGON_CYAN : Constants::HUD_BORDER;

        DrawRectangleRounded(btnRec, 0.2f, 4, btnBg);
        DrawRectangleRoundedLinesEx(btnRec, 0.2f, 4, launchFocused ? 2.0f : 1.0f, btnBorder);

        const char* launchMsg = isCompleted ? "[ENTER / SPACE] REPLAY SECTOR" : "[ENTER / SPACE] COMMENCE MISSION";
        DrawText(launchMsg, (int)btnRec.x + 25, (int)btnRec.y + 8, 14, btnBorder);
    }
}
