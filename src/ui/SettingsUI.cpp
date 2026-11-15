#include "SettingsUI.hpp"
#include "../core/Constants.hpp"
#include <cmath>
#include <string>
#include <algorithm>

SettingsUI::SettingsUI()
    : m_selectedCategory(0)
    , m_selectedItem(0)
    , m_animTime(0.0f)
{
}

bool SettingsUI::Update(GameSettings& settings, SaveData& saveData, bool& requestSave) {
    m_animTime += GetFrameTime();
    bool changed = false;

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        saveData.settings = settings;
        SaveSystem::Save(saveData);
        requestSave = true;
        return true;
    }

    // Category Tabs: Q / E
    if (IsKeyPressed(KEY_Q)) {
        m_selectedCategory = (m_selectedCategory - 1 + 3) % 3;
        m_selectedItem = 0;
    }
    if (IsKeyPressed(KEY_E)) {
        m_selectedCategory = (m_selectedCategory + 1) % 3;
        m_selectedItem = 0;
    }

    int itemCount = (m_selectedCategory == 0) ? 5 : ((m_selectedCategory == 1) ? 5 : 4);
    // Keyboard focus includes the visible SAVE & RETURN control as its final item.
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        m_selectedItem = (m_selectedItem - 1 + itemCount + 1) % (itemCount + 1);
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        m_selectedItem = (m_selectedItem + 1) % (itemCount + 1);
    }

    if (m_selectedItem == itemCount) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            saveData.settings = settings;
            SaveSystem::Save(saveData);
            requestSave = true;
            return true;
        }
    }

    if (m_selectedItem < itemCount && m_selectedCategory == 0) {
        if (m_selectedItem == 0) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) { settings.masterVolume = std::max(0.0f, settings.masterVolume - 0.05f); changed = true; }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) { settings.masterVolume = std::min(1.0f, settings.masterVolume + 0.05f); changed = true; }
        } else if (m_selectedItem == 1) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) { settings.musicVolume = std::max(0.0f, settings.musicVolume - 0.05f); changed = true; }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) { settings.musicVolume = std::min(1.0f, settings.musicVolume + 0.05f); changed = true; }
        } else if (m_selectedItem == 2) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) { settings.sfxVolume = std::max(0.0f, settings.sfxVolume - 0.05f); changed = true; }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) { settings.sfxVolume = std::min(1.0f, settings.sfxVolume + 0.05f); changed = true; }
        } else if (m_selectedItem == 3) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                settings.musicEnabled = !settings.musicEnabled;
                changed = true;
            }
        } else if (m_selectedItem == 4) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                settings.sfxEnabled = !settings.sfxEnabled;
                changed = true;
            }
        }
    } else if (m_selectedItem < itemCount && m_selectedCategory == 1) {
        if (m_selectedItem == 0) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) { settings.screenShakeIntensity = std::max(0.0f, settings.screenShakeIntensity - 0.1f); changed = true; }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) { settings.screenShakeIntensity = std::min(1.5f, settings.screenShakeIntensity + 0.1f); changed = true; }
        } else if (m_selectedItem == 1) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) { settings.visualEffectIntensity = std::max(0.2f, settings.visualEffectIntensity - 0.1f); changed = true; }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) { settings.visualEffectIntensity = std::min(1.5f, settings.visualEffectIntensity + 0.1f); changed = true; }
        } else if (m_selectedItem == 2) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) { settings.colorPalette = (settings.colorPalette - 1 + 3) % 3; changed = true; }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_ENTER)) { settings.colorPalette = (settings.colorPalette + 1) % 3; changed = true; }
        } else if (m_selectedItem == 3) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) { settings.brightness = std::max(0.8f, settings.brightness - 0.05f); changed = true; }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) { settings.brightness = std::min(1.2f, settings.brightness + 0.05f); changed = true; }
        } else if (m_selectedItem == 4) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                settings.vsync = !settings.vsync;
                changed = true;
            }
        }
    } else if (m_selectedItem < itemCount && m_selectedCategory == 2) {
        if (m_selectedItem == 0) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) { settings.difficulty = (settings.difficulty - 1 + 3) % 3; changed = true; }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_ENTER)) { settings.difficulty = (settings.difficulty + 1) % 3; changed = true; }
        } else if (m_selectedItem == 1) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                settings.subtitlesEnabled = !settings.subtitlesEnabled;
                changed = true;
            }
        } else if (m_selectedItem == 2) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                settings.damageNumbers = !settings.damageNumbers;
                changed = true;
            }
        } else if (m_selectedItem == 3) {
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) { settings.cameraTrackingSpeed = std::max(0.5f, settings.cameraTrackingSpeed - 0.1f); changed = true; }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) { settings.cameraTrackingSpeed = std::min(1.5f, settings.cameraTrackingSpeed + 0.1f); changed = true; }
        }
    }

    if (changed) {
        saveData.settings = settings;
        SaveSystem::Save(saveData);
        requestSave = true;
    }

    return false;
}

void SettingsUI::Draw(const GameSettings& settings) const {
    DrawRectangleGradientV(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT,
        { 12, 16, 26, 255 }, { 20, 26, 44, 255 });

    const char* header = "TACTICAL SYSTEM CONFIGURATION";
    int hw = MeasureText(header, 32);
    DrawText(header, (Constants::SCREEN_WIDTH - hw) / 2 + 2, 28, 32, { 180, 20, 30, 160 });
    DrawText(header, (Constants::SCREEN_WIDTH - hw) / 2, 26, 32, Constants::AUTOBOT_RED);

    const char* tabs[3] = { "AUDIO", "VISUALS", "GAMEPLAY" };
    float tabStartX = 320.0f;
    for (int t = 0; t < 3; ++t) {
        float tx = tabStartX + t * 220.0f;
        Rectangle tabRec = { tx, 74.0f, 190.0f, 36.0f };
        bool isCurrentTab = (t == m_selectedCategory);

        Color tabBg = isCurrentTab ? Color{ 28, 48, 76, 240 } : Color{ 16, 22, 34, 200 };
        Color textCol = isCurrentTab ? Constants::ENERGON_CYAN : GRAY;

        DrawRectangleRounded(tabRec, 0.2f, 4, tabBg);
        if (isCurrentTab) {
            DrawRectangleRoundedLinesEx(tabRec, 0.2f, 4, 2.0f, Constants::ENERGON_CYAN);
        }
        int tw = MeasureText(tabs[t], 16);
        DrawText(tabs[t], (int)(tx + (190.0f - tw) / 2), 84, 16, textCol);
    }

    Rectangle contentArea = { 260.0f, 130.0f, Constants::SCREEN_WIDTH - 520.0f, 480.0f };
    DrawRectangle(0, 116, Constants::SCREEN_WIDTH, 2,
        ColorAlpha(Constants::ENERGON_CYAN, 0.35f + 0.2f * std::sin(m_animTime * 2.0f)));
    DrawRectangleRounded(contentArea, 0.05f, 4, { 16, 22, 34, 230 });
    DrawRectangleRoundedLinesEx(contentArea, 0.05f, 4, 1.5f, Constants::HUD_BORDER);

    if (m_selectedCategory == 0) {
        DrawAudioTab(settings, contentArea);
    } else if (m_selectedCategory == 1) {
        DrawVisualTab(settings, contentArea);
    } else {
        DrawGameplayTab(settings, contentArea);
    }

    // Keyboard-only navigation footer and focused save control.
    DrawRectangle(0, Constants::SCREEN_HEIGHT - 38, Constants::SCREEN_WIDTH, 38, { 10, 14, 22, 240 });
    const char* footer = "[Q / E] CHANGE TAB   [W / S] SELECT   [A / D] ADJUST   [ENTER] TOGGLE / SAVE   [ESC] SAVE & RETURN";
    DrawText(footer, 24, Constants::SCREEN_HEIGHT - 26, 12, RAYWHITE);

    Rectangle saveBtnRec = { (float)Constants::SCREEN_WIDTH - 200.0f, (float)Constants::SCREEN_HEIGHT - 72.0f, 170.0f, 28.0f };
    const int itemCount = (m_selectedCategory == 0) ? 5 : ((m_selectedCategory == 1) ? 5 : 4);
    const bool saveFocused = (m_selectedItem == itemCount);
    DrawRectangleRounded(saveBtnRec, 0.2f, 4, saveFocused ? Color{ 25, 65, 90, 245 } : Color{ 18, 32, 48, 220 });
    DrawRectangleRoundedLinesEx(saveBtnRec, 0.2f, 4, saveFocused ? 2.5f : 1.0f, saveFocused ? Constants::ENERGON_CYAN : GRAY);
    DrawText("SAVE & RETURN >", (int)saveBtnRec.x + 22, (int)saveBtnRec.y + 7, 13, RAYWHITE);
}

void SettingsUI::DrawAudioTab(const GameSettings& settings, Rectangle area) const {
    const char* labels[5] = {
        "Master Volume",
        "Music Volume",
        "Sound Effects Volume",
        "Music Output",
        "Sound Effects Output"
    };

    float startY = area.y + 40.0f;
    float rowHeight = 65.0f;

    for (int i = 0; i < 5; ++i) {
        float y = startY + i * rowHeight;
        bool isSel = (i == m_selectedItem);

        if (isSel) {
            const unsigned char pulseAlpha = static_cast<unsigned char>(150 + 45 * (std::sin(m_animTime * 5.0f) + 1.0f) * 0.5f);
            DrawRectangle((int)area.x + 15, (int)y - 5, (int)area.width - 30, 48, { 25, 42, 65, pulseAlpha });
            DrawRectangle((int)area.x + 15, (int)y - 5, 4, 48, Constants::ENERGON_CYAN);
        }

        DrawText(labels[i], (int)area.x + 35, (int)y + 8, 16, isSel ? RAYWHITE : LIGHTGRAY);

        if (i < 3) {
            float val = (i == 0) ? settings.masterVolume : ((i == 1) ? settings.musicVolume : settings.sfxVolume);
            float barW = 200.0f;
            float barX = area.x + area.width - 260.0f;
            DrawRectangle((int)barX, (int)y + 12, (int)barW, 10, { 30, 38, 52, 255 });
            DrawRectangle((int)barX, (int)y + 12, (int)(barW * val), 10, isSel ? Constants::ENERGON_CYAN : Constants::AUTOBOT_BLUE);
            DrawRectangleLines((int)barX, (int)y + 12, (int)barW, 10, GRAY);

            std::string pct = std::to_string((int)(val * 100.0f)) + "%";
            DrawText(pct.c_str(), (int)barX + (int)barW + 15, (int)y + 8, 16, RAYWHITE);
        } else {
            bool enabled = (i == 3) ? settings.musicEnabled : settings.sfxEnabled;
            const char* stateText = enabled ? "[ENABLED]" : "[DISABLED]";
            Color stateColor = enabled ? Color{ 80, 240, 120, 255 } : Color{ 240, 70, 70, 255 };
            DrawText(stateText, (int)(area.x + area.width - 160.0f), (int)y + 8, 16, stateColor);
        }
    }
}

void SettingsUI::DrawVisualTab(const GameSettings& settings, Rectangle area) const {
    const char* labels[5] = {
        "Screen Shake Trauma",
        "Visual Effects Intensity",
        "Palette Theme",
        "Display Brightness",
        "Vertical Sync (VSync)"
    };

    float startY = area.y + 40.0f;
    float rowHeight = 65.0f;

    for (int i = 0; i < 5; ++i) {
        float y = startY + i * rowHeight;
        bool isSel = (i == m_selectedItem);

        if (isSel) {
            const unsigned char pulseAlpha = static_cast<unsigned char>(150 + 45 * (std::sin(m_animTime * 5.0f) + 1.0f) * 0.5f);
            DrawRectangle((int)area.x + 15, (int)y - 5, (int)area.width - 30, 48, { 25, 42, 65, pulseAlpha });
            DrawRectangle((int)area.x + 15, (int)y - 5, 4, 48, Constants::ENERGON_CYAN);
        }

        DrawText(labels[i], (int)area.x + 35, (int)y + 8, 16, isSel ? RAYWHITE : LIGHTGRAY);

        if (i == 0 || i == 1) {
            float val = (i == 0) ? (settings.screenShakeIntensity / 1.5f) : (settings.visualEffectIntensity / 1.5f);
            float barW = 200.0f;
            float barX = area.x + area.width - 260.0f;
            DrawRectangle((int)barX, (int)y + 12, (int)barW, 10, { 30, 38, 52, 255 });
            DrawRectangle((int)barX, (int)y + 12, (int)(barW * val), 10, isSel ? Constants::ENERGON_CYAN : Constants::AUTOBOT_BLUE);
            DrawRectangleLines((int)barX, (int)y + 12, (int)barW, 10, GRAY);
        } else if (i == 2) {
            const char* palNames[3] = { "Cybertron Indigo", "High Contrast Neon", "Deep Space" };
            DrawText(palNames[settings.colorPalette], (int)(area.x + area.width - 230.0f), (int)y + 8, 16, Constants::ENERGON_CYAN);
        } else if (i == 3) {
            std::string brStr = std::to_string((int)(settings.brightness * 100.0f)) + "%";
            DrawText(brStr.c_str(), (int)(area.x + area.width - 150.0f), (int)y + 8, 16, RAYWHITE);
        } else {
            const char* vsyncText = settings.vsync ? "[ON]" : "[OFF]";
            DrawText(vsyncText, (int)(area.x + area.width - 140.0f), (int)y + 8, 16, settings.vsync ? Color{ 80, 240, 120, 255 } : Color{ 240, 70, 70, 255 });
        }
    }
}

void SettingsUI::DrawGameplayTab(const GameSettings& settings, Rectangle area) const {
    const char* labels[4] = {
        "Combat Difficulty",
        "Dialogue Subtitles",
        "Damage Floating Text",
        "Camera Dynamic Tracking"
    };

    float startY = area.y + 40.0f;
    float rowHeight = 65.0f;

    for (int i = 0; i < 4; ++i) {
        float y = startY + i * rowHeight;
        bool isSel = (i == m_selectedItem);

        if (isSel) {
            const unsigned char pulseAlpha = static_cast<unsigned char>(150 + 45 * (std::sin(m_animTime * 5.0f) + 1.0f) * 0.5f);
            DrawRectangle((int)area.x + 15, (int)y - 5, (int)area.width - 30, 48, { 25, 42, 65, pulseAlpha });
            DrawRectangle((int)area.x + 15, (int)y - 5, 4, 48, Constants::ENERGON_CYAN);
        }

        DrawText(labels[i], (int)area.x + 35, (int)y + 8, 16, isSel ? RAYWHITE : LIGHTGRAY);

        if (i == 0) {
            const char* diffNames[3] = { "Recruit (Easy)", "Warrior (Normal)", "Prime Veteran (Hard)" };
            Color diffCols[3] = { Color{ 80, 220, 120, 255 }, Constants::ENERGON_CYAN, Constants::AUTOBOT_RED };
            DrawText(diffNames[settings.difficulty], (int)(area.x + area.width - 230.0f), (int)y + 8, 16, diffCols[settings.difficulty]);
        } else if (i == 1) {
            const char* subText = settings.subtitlesEnabled ? "[ENABLED]" : "[DISABLED]";
            DrawText(subText, (int)(area.x + area.width - 160.0f), (int)y + 8, 16, settings.subtitlesEnabled ? Color{ 80, 240, 120, 255 } : Color{ 240, 70, 70, 255 });
        } else if (i == 2) {
            const char* dmgText = settings.damageNumbers ? "[ENABLED]" : "[DISABLED]";
            DrawText(dmgText, (int)(area.x + area.width - 160.0f), (int)y + 8, 16, settings.damageNumbers ? Color{ 80, 240, 120, 255 } : Color{ 240, 70, 70, 255 });
        } else {
            std::string camStr = std::to_string((int)(settings.cameraTrackingSpeed * 100.0f)) + "%";
            DrawText(camStr.c_str(), (int)(area.x + area.width - 150.0f), (int)y + 8, 16, RAYWHITE);
        }
    }
}
