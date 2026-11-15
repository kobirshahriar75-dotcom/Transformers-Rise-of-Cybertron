#include "MenuSystem.hpp"
#include <cmath>
#include <string>

MenuSystem::MenuSystem()
    : m_selectedItem(0)
    , m_lastState(GameState::OPENING_CINEMATIC)
    , m_showControlsModal(false)
    , m_showCreditsModal(false)
    , m_animTime(0.0f)
{
}

void MenuSystem::Update(GameState& state, const SaveData& saveData, bool& requestRestart, bool& requestQuit, int& outStartLevel) {
    if (state != m_lastState) {
        m_selectedItem = (state == GameState::MENU && saveData.highestUnlockedLevel > 1) ? 1 : 0;
        m_lastState = state;
    }
    m_animTime += GetFrameTime();

    // Modal dialogs have one keyboard action: confirm or cancel to close.
    if (m_showControlsModal || m_showCreditsModal) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) ||
            IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            m_showControlsModal = false;
            m_showCreditsModal = false;
        }
        return;
    }

    const bool up = IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W);
    const bool down = IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S);
    const bool confirm = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);

    if (state == GameState::MENU) {
        constexpr int itemCount = 7;
        if (up) m_selectedItem = (m_selectedItem - 1 + itemCount) % itemCount;
        if (down) m_selectedItem = (m_selectedItem + 1) % itemCount;
        if (!confirm) return;

        switch (m_selectedItem) {
            case 0: outStartLevel = 1; state = GameState::PLAYING; requestRestart = true; break;
            case 1: outStartLevel = saveData.highestUnlockedLevel; state = GameState::PLAYING; requestRestart = true; break;
            case 2: state = GameState::LEVEL_SELECT; break;
            case 3: state = GameState::SETTINGS; break;
            case 4: m_showControlsModal = true; break;
            case 5: m_showCreditsModal = true; break;
            case 6: requestQuit = true; break;
        }
        return;
    }

    if (state == GameState::PAUSED) {
        constexpr int itemCount = 3;
        if (up) m_selectedItem = (m_selectedItem - 1 + itemCount) % itemCount;
        if (down) m_selectedItem = (m_selectedItem + 1) % itemCount;
        if (!confirm) return;

        if (m_selectedItem == 0) state = GameState::PLAYING;
        else if (m_selectedItem == 1) { state = GameState::PLAYING; requestRestart = true; }
        else { state = GameState::MENU; m_selectedItem = 0; }
        return;
    }

    if (state == GameState::GAME_OVER) {
        constexpr int itemCount = 2;
        if (up) m_selectedItem = (m_selectedItem - 1 + itemCount) % itemCount;
        if (down) m_selectedItem = (m_selectedItem + 1) % itemCount;
        if (!confirm) return;

        if (m_selectedItem == 0) {
            state = GameState::PLAYING;
            requestRestart = true;
        } else {
            state = GameState::LEVEL_SELECT;
            m_selectedItem = 0;
        }
        return;
    }

    // Retain keyboard support for the legacy two-option victory presentation.
    // The active campaign flow currently uses MissionCompletionUI instead.
    if (state == GameState::VICTORY) {
        constexpr int itemCount = 2;
        if (up) m_selectedItem = (m_selectedItem - 1 + itemCount) % itemCount;
        if (down) m_selectedItem = (m_selectedItem + 1) % itemCount;
        if (!confirm) return;

        if (m_selectedItem == 0) {
            outStartLevel = saveData.highestUnlockedLevel;
            state = GameState::PLAYING;
            requestRestart = true;
        } else {
            state = GameState::LEVEL_SELECT;
            m_selectedItem = 0;
        }
    }
}

void MenuSystem::DrawMainMenu(const SaveData& saveData) const {
    DrawRectangleGradientV(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT,
        { 12, 16, 28, 255 }, { 24, 32, 54, 255 });

    for (int y = 0; y < Constants::SCREEN_HEIGHT; y += 40) {
        DrawLine(0, y, Constants::SCREEN_WIDTH, y, { 35, 45, 75, 45 });
    }
    for (int x = 0; x < Constants::SCREEN_WIDTH; x += 80) {
        DrawLine(x, 0, x, Constants::SCREEN_HEIGHT, { 35, 45, 75, 25 });
    }
    const float scanY = std::fmod(m_animTime * 42.0f, (float)Constants::SCREEN_HEIGHT);
    DrawLine(0, (int)scanY, Constants::SCREEN_WIDTH, (int)scanY, { 0, 210, 255, 24 });
    DrawRectangle(34, 34, 5, 92, Constants::AUTOBOT_RED);
    DrawRectangle(41, 34, 2, 92, ColorAlpha(Constants::ENERGON_CYAN, 0.55f));
    DrawText("AUTOBOT COMMAND // SECURE LINK", 58, 38, 11, Constants::ENERGON_CYAN);

    const char* titleLine1 = "TRANSFORMERS";
    const char* titleLine2 = "RISE OF CYBERTRON";
    const char* subtitle = "COMMERCIAL GRADE C++ / RAYLIB ACTION BRAWLER";

    int w1 = MeasureText(titleLine1, 54);
    int w2 = MeasureText(titleLine2, 38);
    int wSub = MeasureText(subtitle, 14);

    float titleY = 60.0f;
    DrawText(titleLine1, (Constants::SCREEN_WIDTH - w1) / 2 + 2, (int)titleY + 2, 54, { 180, 20, 30, 180 });
    DrawText(titleLine1, (Constants::SCREEN_WIDTH - w1) / 2, (int)titleY, 54, Constants::AUTOBOT_RED);

    DrawText(titleLine2, (Constants::SCREEN_WIDTH - w2) / 2 + 2, (int)titleY + 62, 38, { 0, 160, 210, 160 });
    DrawText(titleLine2, (Constants::SCREEN_WIDTH - w2) / 2, (int)titleY + 60, 38, Constants::ENERGON_CYAN);

    DrawText(subtitle, (Constants::SCREEN_WIDTH - wSub) / 2, (int)titleY + 110, 14, Constants::AUTOBOT_SILVER);

    // 7 Options - NO Extras
    const char* options[7] = {
        "NEW CAMPAIGN",
        "CONTINUE CAMPAIGN",
        "LEVEL SELECTION",
        "SETTINGS",
        "CONTROLS",
        "CREDITS",
        "EXIT"
    };

    float startY = 240.0f;
    float itemHeight = 48.0f;
    float boxWidth = 420.0f;
    float boxX = (Constants::SCREEN_WIDTH - boxWidth) / 2.0f;

    for (int i = 0; i < 7; i++) {
        float itemY = startY + i * itemHeight;
        bool isSelected = (i == m_selectedItem);

        if (isSelected) {
            float pulse = (sinf(m_animTime * 6.0f) + 1.0f) * 0.5f;
            Color highlightBg = { 35, 55, 90, (unsigned char)(180 + pulse * 60) };

            DrawRectangleRounded({ boxX, itemY, boxWidth, 40.0f }, 0.25f, 4, highlightBg);
            DrawRectangleRoundedLinesEx({ boxX, itemY, boxWidth, 40.0f }, 0.25f, 4, 2.0f, Constants::ENERGON_CYAN);

            DrawTriangle(
                { boxX + 16.0f, itemY + 12.0f },
                { boxX + 26.0f, itemY + 20.0f },
                { boxX + 16.0f, itemY + 28.0f },
                Constants::AUTOBOT_RED
            );
        } else {
            DrawRectangleRounded({ boxX, itemY, boxWidth, 40.0f }, 0.25f, 4, { 18, 22, 35, 140 });
        }

        std::string label = options[i];
        if (i == 1) {
            label += " (SECTOR " + std::to_string(saveData.highestUnlockedLevel) + ")";
        }

        int textW = MeasureText(label.c_str(), 16);
        DrawText(label.c_str(), (int)(boxX + (boxWidth - textW) / 2.0f), (int)itemY + 11, 16, isSelected ? RAYWHITE : LIGHTGRAY);
    }

    const char* footer = "[ARROWS / W,A,S,D] NAVIGATE    |    [ENTER / SPACE] SELECT";
    int fw = MeasureText(footer, 13);
    DrawText(footer, (Constants::SCREEN_WIDTH - fw) / 2, Constants::SCREEN_HEIGHT - 30, 13, Constants::AUTOBOT_SILVER);
}

void MenuSystem::DrawPauseMenu() const {
    DrawRectangle(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, { 5, 8, 16, 195 });

    float boxW = 380.0f;
    float boxH = 260.0f;
    float boxX = (Constants::SCREEN_WIDTH - boxW) / 2.0f;
    float boxY = (Constants::SCREEN_HEIGHT - boxH) / 2.0f;

    DrawRectangleRounded({ boxX, boxY, boxW, boxH }, 0.15f, 4, Constants::HUD_BG);
    DrawRectangleRoundedLinesEx({ boxX, boxY, boxW, boxH }, 0.15f, 4, 2.0f, Constants::ENERGON_CYAN);

    const char* pauseTitle = "SYSTEM PAUSED";
    int pw = MeasureText(pauseTitle, 28);
    DrawText(pauseTitle, (int)(boxX + (boxW - pw) / 2.0f), (int)boxY + 25, 28, Constants::AUTOBOT_RED);

    const char* items[3] = { "RESUME COMBAT", "RESTART LEVEL", "RETURN TO MAIN MENU" };
    for (int i = 0; i < 3; i++) {
        float iy = boxY + 85.0f + i * 48.0f;
        bool isSel = (i == m_selectedItem);

        if (isSel) {
            const float pulse = (sinf(m_animTime * 5.0f) + 1.0f) * 0.5f;
            DrawRectangleRounded({ boxX + 25.0f, iy, boxW - 50.0f, 38.0f }, 0.2f, 4,
                Color{ 25, 55, 90, (unsigned char)(190 + 45 * pulse) });
            DrawRectangleRoundedLinesEx({ boxX + 25.0f, iy, boxW - 50.0f, 38.0f }, 0.2f, 4, 2.0f, Constants::ENERGON_CYAN);
        }
        int tw = MeasureText(items[i], 16);
        DrawText(items[i], (int)(boxX + (boxW - tw) / 2.0f), (int)iy + 10, 16, isSel ? RAYWHITE : LIGHTGRAY);
    }
    const char* hint = "[UP / DOWN] SELECT    [ENTER / SPACE] CONFIRM";
    const int hintW = MeasureText(hint, 12);
    DrawText(hint, (Constants::SCREEN_WIDTH - hintW) / 2, Constants::SCREEN_HEIGHT - 26, 12, Constants::AUTOBOT_SILVER);
}

void MenuSystem::DrawGameOver(int score, int kills, int wave) const {
    DrawRectangle(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, { 25, 5, 8, 220 });

    float boxW = 460.0f;
    float boxH = 320.0f;
    float boxX = (Constants::SCREEN_WIDTH - boxW) / 2.0f;
    float boxY = (Constants::SCREEN_HEIGHT - boxH) / 2.0f;

    DrawRectangleRounded({ boxX, boxY, boxW, boxH }, 0.12f, 4, Constants::HUD_BG);
    DrawRectangleRoundedLinesEx({ boxX, boxY, boxW, boxH }, 0.12f, 4, 2.0f, Constants::AUTOBOT_RED);

    const char* title = "MISSION FAILED";
    int tw = MeasureText(title, 36);
    DrawText(title, (int)(boxX + (boxW - tw) / 2.0f), (int)boxY + 24, 36, Constants::AUTOBOT_RED);

    std::string s1 = "Enemies Eliminated: " + std::to_string(kills);
    std::string s2 = "Wave Reached: " + std::to_string(wave);
    std::string s3 = "Final Combat Score: " + std::to_string(score);

    DrawText(s1.c_str(), (int)boxX + 50, (int)boxY + 85, 16, RAYWHITE);
    DrawText(s2.c_str(), (int)boxX + 50, (int)boxY + 115, 16, RAYWHITE);
    DrawText(s3.c_str(), (int)boxX + 50, (int)boxY + 145, 18, Constants::ENERGON_CYAN);

    const char* opts[2] = { "RETRY SECTOR", "TACTICAL MAP (LEVEL SELECT)" };
    for (int i = 0; i < 2; i++) {
        float iy = boxY + 195.0f + i * 48.0f;
        bool isSel = (i == m_selectedItem);

        if (isSel) {
            const float pulse = (sinf(m_animTime * 5.0f) + 1.0f) * 0.5f;
            DrawRectangleRounded({ boxX + 30.0f, iy, boxW - 60.0f, 38.0f }, 0.2f, 4,
                Color{ 55, 20, 30, (unsigned char)(185 + 45 * pulse) });
            DrawRectangleRoundedLinesEx({ boxX + 30.0f, iy, boxW - 60.0f, 38.0f }, 0.2f, 4, 2.0f, Constants::AUTOBOT_RED);
        }
        int ow = MeasureText(opts[i], 16);
        DrawText(opts[i], (int)(boxX + (boxW - ow) / 2.0f), (int)iy + 10, 16, isSel ? RAYWHITE : LIGHTGRAY);
    }
    const char* hint = "[UP / DOWN] SELECT    [ENTER / SPACE] CONFIRM";
    const int hintW = MeasureText(hint, 12);
    DrawText(hint, (Constants::SCREEN_WIDTH - hintW) / 2, Constants::SCREEN_HEIGHT - 26, 12, Constants::AUTOBOT_SILVER);
}

void MenuSystem::DrawVictory(int score, int kills) const {
    DrawRectangle(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, { 5, 20, 30, 215 });

    float boxW = 500.0f;
    float boxH = 330.0f;
    float boxX = (Constants::SCREEN_WIDTH - boxW) / 2.0f;
    float boxY = (Constants::SCREEN_HEIGHT - boxH) / 2.0f;

    DrawRectangleRounded({ boxX, boxY, boxW, boxH }, 0.12f, 4, Constants::HUD_BG);
    DrawRectangleRoundedLinesEx({ boxX, boxY, boxW, boxH }, 0.12f, 4, 2.5f, Constants::ENERGON_CYAN);

    const char* title = "SECTOR SECURED!";
    int tw = MeasureText(title, 36);
    DrawText(title, (int)(boxX + (boxW - tw) / 2.0f), (int)boxY + 22, 36, Constants::ENERGON_CYAN);

    const char* sub = "DECEPTICON FORCES NEUTRALIZED";
    int sw = MeasureText(sub, 14);
    DrawText(sub, (int)(boxX + (boxW - sw) / 2.0f), (int)boxY + 64, 14, Constants::AUTOBOT_SILVER);

    std::string s1 = "Decepticons Vanquished: " + std::to_string(kills);
    std::string s2 = "Total Combat Score: " + std::to_string(score);
    std::string s3 = "Next Campaign Sector Unlocked!";

    DrawText(s1.c_str(), (int)boxX + 50, (int)boxY + 100, 16, RAYWHITE);
    DrawText(s2.c_str(), (int)boxX + 50, (int)boxY + 128, 18, { 255, 215, 60, 255 });
    DrawText(s3.c_str(), (int)boxX + 50, (int)boxY + 158, 15, { 100, 240, 140, 255 });

    const char* opts[2] = { "CONTINUE / REPLAY", "LEVEL SELECT / CAMPAIGN MAP" };
    for (int i = 0; i < 2; i++) {
        float iy = boxY + 205.0f + i * 48.0f;
        bool isSel = (i == m_selectedItem);

        if (isSel) {
            DrawRectangleRounded({ boxX + 30.0f, iy, boxW - 60.0f, 38.0f }, 0.2f, 4, { 20, 50, 75, 220 });
            DrawRectangleRoundedLinesEx({ boxX + 30.0f, iy, boxW - 60.0f, 38.0f }, 0.2f, 4, 1.5f, Constants::ENERGON_CYAN);
        }
        int ow = MeasureText(opts[i], 16);
        DrawText(opts[i], (int)(boxX + (boxW - ow) / 2.0f), (int)iy + 10, 16, isSel ? RAYWHITE : LIGHTGRAY);
    }
    const char* hint = "[UP / DOWN] SELECT    [ENTER / SPACE] CONFIRM";
    const int hintW = MeasureText(hint, 12);
    DrawText(hint, (Constants::SCREEN_WIDTH - hintW) / 2, Constants::SCREEN_HEIGHT - 26, 12, Constants::AUTOBOT_SILVER);
}

void MenuSystem::DrawControlsModal() const {
    DrawRectangle(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, { 0, 0, 0, 210 });

    float w = 680.0f;
    float h = 500.0f;
    float x = (Constants::SCREEN_WIDTH - w) / 2.0f;
    float y = (Constants::SCREEN_HEIGHT - h) / 2.0f;

    DrawRectangleRounded({ x, y, w, h }, 0.1f, 4, Constants::HUD_BG);
    DrawRectangleRoundedLinesEx({ x, y, w, h }, 0.1f, 4, 2.0f, Constants::HUD_BORDER);

    const char* title = "TACTICAL CONTROLS PROTOCOL";
    int tw = MeasureText(title, 24);
    DrawText(title, (int)(x + (w - tw) / 2.0f), (int)y + 20, 24, Constants::ENERGON_CYAN);

    struct ControlRow {
        const char* action;
        const char* keys;
    };
    ControlRow rows[] = {
        { "Move Left / Right / Fly", "A / D / W / S   or   Arrows" },
        { "Jump / Double-Jump / Jet Ascend", "W   or   Space / Up Arrow" },
        { "Melee Slash / Fire Blaster", "J   or   Z" },
        { "Energon Wave / Overload Blast", "K   or   X" },
        { "Cycle Mode (Blade/Cannon/Truck)", "C   or   Left Shift" },
        { "Direct Mode Select", "1 (Blade) | 2 (Cannon) | 3 (Truck)" },
        { "Truck Nitro Boost", "Space   or   K / X (while in Truck)" },
        { "Truck Semi Air Horn", "H   or   J (while in Truck)" },
        { "Tactical Pause", "P   or   Escape" }
    };

    float ry = y + 65.0f;
    for (const auto& r : rows) {
        DrawText(r.action, (int)x + 40, (int)ry, 14, RAYWHITE);
        DrawText(r.keys, (int)x + 310, (int)ry, 14, Constants::AUTOBOT_SILVER);
        DrawLine((int)x + 35, (int)ry + 20, (int)(x + w - 35), (int)ry + 20, { 40, 50, 70, 150 });
        ry += 36.0f;
    }

    DrawText("MENU NAVIGATION: ARROWS / W,A,S,D     CONFIRM: ENTER / SPACE     BACK: ESC",
             (int)x + 40, (int)y + 414, 12, Constants::ENERGON_CYAN);

    // Keyboard-confirmed close control
    Rectangle closeBtn = { (float)Constants::SCREEN_WIDTH * 0.5f - 80.0f, y + h - 50.0f, 160.0f, 34.0f };
    DrawRectangleRounded(closeBtn, 0.25f, 4, Color{ 25, 45, 68, 240 });
    DrawRectangleRoundedLinesEx(closeBtn, 0.25f, 4, 2.0f, Constants::ENERGON_CYAN);
    const char* closeHint = "[ENTER / ESC] CLOSE PROTOCOL";
    int cw = MeasureText(closeHint, 14);
    DrawText(closeHint, (int)(closeBtn.x + (closeBtn.width - cw) / 2), (int)closeBtn.y + 9, 14, RAYWHITE);
}

void MenuSystem::DrawCreditsModal() const {
    DrawRectangle(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, { 0, 0, 0, 220 });

    float w = 620.0f;
    float h = 420.0f;
    float x = (Constants::SCREEN_WIDTH - w) / 2.0f;
    float y = (Constants::SCREEN_HEIGHT - h) / 2.0f;

    DrawRectangleRounded({ x, y, w, h }, 0.1f, 4, Constants::HUD_BG);
    DrawRectangleRoundedLinesEx({ x, y, w, h }, 0.1f, 4, 2.0f, Constants::AUTOBOT_RED);

    const char* title = "TRANSFORMERS: RISE OF CYBERTRON";
    int tw = MeasureText(title, 24);
    DrawText(title, (int)(x + (w - tw) / 2.0f), (int)y + 24, 24, Constants::AUTOBOT_RED);

    const char* sub = "CREDITS & INTELLECTUAL PROPERTY";
    int sw = MeasureText(sub, 14);
    DrawText(sub, (int)(x + (w - sw) / 2.0f), (int)y + 56, 14, Constants::ENERGON_CYAN);

    DrawText("Core Architecture & Development: Senior C++ / Raylib Game Team", (int)x + 40, (int)y + 100, 14, RAYWHITE);
    DrawText("Rendering Framework: Raylib 6.2 (Procedural Shading & Mesh Articulation)", (int)x + 40, (int)y + 130, 14, LIGHTGRAY);
    DrawText("Audio Engine: Centralized Synthesizer & Procedural Wave Generation", (int)x + 40, (int)y + 160, 14, LIGHTGRAY);
    DrawText("Character Design: Live-Action / Bayverse Aesthetic (Raylib Procedural)", (int)x + 40, (int)y + 190, 14, LIGHTGRAY);
    DrawText("Inspiration: Transformers Universe (Hasbro / Paramount Pictures)", (int)x + 40, (int)y + 230, 14, { 255, 215, 60, 255 });
    DrawText("Soundtrack Identity: Transformers Prime (Brian Tyler tribute)", (int)x + 40, (int)y + 260, 14, { 255, 215, 60, 255 });
    DrawText("Notice: Commercial release requires appropriate rights-holder authorization.", (int)x + 40, (int)y + 300, 12, GRAY);

    // Keyboard-confirmed close control
    Rectangle closeBtn = { (float)Constants::SCREEN_WIDTH * 0.5f - 80.0f, y + h - 50.0f, 160.0f, 34.0f };
    DrawRectangleRounded(closeBtn, 0.25f, 4, Color{ 25, 45, 68, 240 });
    DrawRectangleRoundedLinesEx(closeBtn, 0.25f, 4, 2.0f, Constants::ENERGON_CYAN);
    const char* closeHint = "[ENTER / ESC] CLOSE CREDITS";
    int cw = MeasureText(closeHint, 14);
    DrawText(closeHint, (int)(closeBtn.x + (closeBtn.width - cw) / 2), (int)closeBtn.y + 9, 14, RAYWHITE);
}

void MenuSystem::Draw(GameState state, const SaveData& saveData, int score, int kills, int wave) {
    // Drawing can be the first frame after an external state transition. Reset
    // here as well as in Update so the correct button is highlighted immediately.
    if (state != m_lastState) {
        m_selectedItem = (state == GameState::MENU && saveData.highestUnlockedLevel > 1) ? 1 : 0;
        m_lastState = state;
    }
    if (m_showControlsModal) {
        DrawControlsModal();
        return;
    }
    if (m_showCreditsModal) {
        DrawCreditsModal();
        return;
    }

    switch (state) {
        case GameState::MENU:
            DrawMainMenu(saveData);
            break;
        case GameState::PAUSED:
            DrawPauseMenu();
            break;
        case GameState::GAME_OVER:
            DrawGameOver(score, kills, wave);
            break;
        case GameState::VICTORY:
            DrawVictory(score, kills);
            break;
        default:
            break;
    }
}
