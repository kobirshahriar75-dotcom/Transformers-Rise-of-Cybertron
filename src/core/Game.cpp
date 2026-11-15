#include "Game.hpp"
#include <algorithm>
#include <cmath>

Game::Game()
    : m_state(GameState::OPENING_CINEMATIC)
    , m_player(1, Vector2{ 200.0f, Constants::GROUND_Y })
    , m_shouldQuit(false)
    , m_victoryRegistered(false)
    , m_cinematicTimer(0.0f)
    , m_levelElapsedTime(0.0f)
    , m_completionCoresEarned(0)
{
}

Game::~Game() {
    Shutdown();
}

void Game::ApplySettings() {
    m_sounds.SetBgmVolume(m_saveData.settings.musicVolume * (m_saveData.settings.musicEnabled ? 1.0f : 0.0f) * m_saveData.settings.masterVolume);
    m_sounds.SetSfxVolume(m_saveData.settings.sfxVolume * (m_saveData.settings.sfxEnabled ? 1.0f : 0.0f) * m_saveData.settings.masterVolume);
}

void Game::Init() {
    InitWindow(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, Constants::WINDOW_TITLE);
    SetTargetFPS(Constants::TARGET_FPS);
    HideCursor(); // The game is intentionally operated entirely by keyboard.

    SaveSystem::Load(m_saveData);

    m_sounds.Init();
    ApplySettings();

    m_bg.Init();
    m_arena.Init();
    m_camera.Init(m_player.GetPosition());

    StartLevel(1);
    m_state = GameState::OPENING_CINEMATIC;
    m_cinematicTimer = 0.0f;
    m_sounds.PlayMusicTrack(MusicTrack::MENU);
}

void Game::Shutdown() {
    SaveSystem::Save(m_saveData);
    m_sounds.Shutdown();
    if (IsWindowReady()) {
        CloseWindow();
    }
}

bool Game::StartLevel(int levelNumber) {
    // Final progression gate: no caller can start a locked or invalid level.
    if (levelNumber < 1 || levelNumber > 10 || !SaveSystem::IsLevelUnlocked(m_saveData, levelNumber)) {
        return false;
    }

    m_arena.SetCurrentLevel(levelNumber);
    const LevelInfo& info = m_arena.GetLevelInfo();
    m_bg.SetTheme(info.themeSkyTop, info.themeSkyBottom, info.neonColor);

    Reset();

    // Levels 7 and 8 are the aerial-combat campaign arc.
    // Level 7 teaches controlled flight; Level 8 carries that skill into
    // the Starscream boss encounter.
    const bool isAerialLevel = (levelNumber == 7 || levelNumber == 8);
    m_player.SetJetpackMode(isAerialLevel);
    if (isAerialLevel) {
        const float aerialSpawnY = (levelNumber == 7) ? 380.0f : 300.0f;
        m_player.SetPosition({ 200.0f, aerialSpawnY });
    }

    m_victoryRegistered = false;
    m_levelElapsedTime = 0.0f;
    m_completionCoresEarned = 0;
    m_sounds.PlayMusicTrack(MusicTrack::GAMEPLAY);
    return true;
}

void Game::Reset() {
    m_player.Reset(Vector2{ 200.0f, Constants::GROUND_Y });
    m_projectiles.clear();
    m_particles.Reset();
    m_combat.Reset();
    m_arena.Reset();
    m_camera.Reset(m_player.GetPosition());
}

void Game::Update(float dt) {
    m_sounds.Update(dt);

    // 1. Opening Cinematic Scene (Plays on game launch)
    if (m_state == GameState::OPENING_CINEMATIC) {
        m_cinematicTimer += dt;

        // Keyboard-only cinematic skip; J is intentionally not used here.
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || m_cinematicTimer > 16.0f) {
            m_state = GameState::MENU;
            m_sounds.PlayMusicTrack(MusicTrack::MENU);
        }
        return;
    }

    // 2. Tactical Level Select
    if (m_state == GameState::LEVEL_SELECT) {
        m_sounds.PlayMusicTrack(MusicTrack::MENU);
        int selectedLevel = 1;
        bool backToMenu = false;
        if (m_levelSelectUI.Update(m_saveData, selectedLevel, backToMenu)) {
            if (StartLevel(selectedLevel)) m_state = GameState::PLAYING;
        } else if (backToMenu) {
            m_state = GameState::MENU;
        }
        return;
    }

    // 3. System Configuration Settings
    if (m_state == GameState::SETTINGS) {
        m_sounds.PlayMusicTrack(MusicTrack::MENU);
        bool reqSave = false;
        bool returnToMenu = m_settingsUI.Update(m_saveData.settings, m_saveData, reqSave);
        if (reqSave) {
            ApplySettings(); // Apply audio toggles and volume changes immediately.
        }
        if (returnToMenu) {
            m_state = GameState::MENU;
        }
        return;
    }

    // 4. Main Menu State
    if (m_state == GameState::MENU) {
        m_sounds.PlayMusicTrack(MusicTrack::MENU);
        bool requestRestart = false;
        bool requestQuit = false;
        int startLevel = 1;

        m_menu.Update(m_state, m_saveData, requestRestart, requestQuit, startLevel);

        if (requestRestart) {
            if (StartLevel(startLevel)) m_state = GameState::PLAYING;
        }
        if (requestQuit) {
            m_shouldQuit = true;
        }
        return;
    }

    // 5. Pause Menu (menu score plays; gameplay score stops)
    if (m_state == GameState::PAUSED) {
        // Pause is a non-gameplay state: use the menu score, not the battle score.
        m_sounds.PlayMusicTrack(MusicTrack::MENU);

        bool requestRestart = false;
        bool requestQuit = false;
        int dummy = 1;
        m_menu.Update(m_state, m_saveData, requestRestart, requestQuit, dummy);

        if (requestRestart) {
            if (StartLevel(m_arena.GetCurrentLevel())) m_state = GameState::PLAYING;
            m_sounds.PlayMusicTrack(MusicTrack::GAMEPLAY);
        } else if (m_state == GameState::PLAYING) {
            // Resume combat with the dedicated gameplay score.
            m_sounds.PlayMusicTrack(MusicTrack::GAMEPLAY);
        } else if (m_state == GameState::MENU) {
            // Player returned to menu from pause
            m_sounds.PlayMusicTrack(MusicTrack::MENU);
        }
        return;
    }

    // 6. Mission completion has a dedicated report and progression flow.
    if (m_state == GameState::VICTORY) {
        m_sounds.PlayMusicTrack(MusicTrack::NONE);
        m_sounds.StopAllSounds();
        const int completedLevel = m_arena.GetCurrentLevel();
        const MissionCompletionUI::Action action = m_missionCompletionUI.Update(m_saveData, completedLevel);
        if (action == MissionCompletionUI::Action::NEXT_LEVEL) {
            const int nextLevel = completedLevel + 1;
            if (SaveSystem::IsLevelUnlocked(m_saveData, nextLevel) && StartLevel(nextLevel)) {
                m_state = GameState::PLAYING;
            }
        } else if (action == MissionCompletionUI::Action::LEVEL_SELECT) {
            m_state = GameState::LEVEL_SELECT;
            m_sounds.PlayMusicTrack(MusicTrack::MENU);
        } else if (action == MissionCompletionUI::Action::REPLAY_LEVEL) {
            if (StartLevel(completedLevel)) m_state = GameState::PLAYING;
        } else if (action == MissionCompletionUI::Action::MAIN_MENU) {
            m_state = GameState::MENU;
            m_sounds.PlayMusicTrack(MusicTrack::MENU);
        }
        return;
    }

    // Defeat continues to use the existing game-over menu without changing its flow.
    if (m_state == GameState::GAME_OVER) {
        m_sounds.PlayMusicTrack(MusicTrack::NONE);
        m_sounds.StopAllSounds();
        bool requestRestart = false;
        bool requestQuit = false;
        int dummy = 1;
        m_menu.Update(m_state, m_saveData, requestRestart, requestQuit, dummy);
        if (requestRestart) {
            if (StartLevel(m_arena.GetCurrentLevel())) m_state = GameState::PLAYING;
        } else if (m_state == GameState::LEVEL_SELECT || m_state == GameState::MENU) {
            m_sounds.PlayMusicTrack(MusicTrack::MENU);
        }
        return;
    }

    // 7. Active Gameplay
    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
        m_menu.ResetSelection();
        m_state = GameState::PAUSED;
        m_sounds.PauseMusic();
        m_sounds.StopAllSounds();
        return;
    }

    m_levelElapsedTime += dt;
    m_bg.Update(dt);

    float hitStop = m_combat.GetHitStopTimer();
    if (hitStop > 0.0f) {
        m_combat.UpdateHitStop(dt);
        m_camera.Update(m_player.GetPosition(), dt);
        m_particles.Update(dt);
        return;
    }

    // Player update
    m_player.HandleInput(m_sounds, m_particles, m_projectiles);
    m_player.Update(dt);

    // Camera update
    m_camera.Update(m_player.GetPosition(), dt);

    // Projectiles
    for (size_t i = 0; i < m_projectiles.size(); ) {
        m_projectiles[i].Update(dt);
        if (m_projectiles[i].HasExpired()) {
            m_projectiles.erase(m_projectiles.begin() + i);
        } else {
            i++;
        }
    }

    // Arena wave & enemy AI updates
    m_arena.Update(dt, m_player.GetPosition(), m_sounds, m_particles);

    for (auto& enemy : m_arena.GetEnemies()) {
        enemy.UpdateAI(m_player.GetPosition(), dt, m_sounds, m_particles, m_projectiles);
        enemy.Update(dt);
    }

    if (m_arena.GetBoss()) {
        Boss* boss = m_arena.GetBoss();
        boss->UpdateAI(m_player.GetPosition(), dt, m_sounds, m_particles, m_projectiles);
        boss->Update(dt);
    }

    // Combat resolution
    m_combat.Update(dt, m_player, m_arena.GetEnemies(), m_arena.GetBoss(), m_projectiles, m_particles, m_camera, m_sounds);

    // Particles
    m_particles.Update(dt);

    // Victory & Defeat
    if (m_player.IsDead()) {
        m_menu.ResetSelection();
        m_state = GameState::GAME_OVER;
        m_sounds.PlayMusicTrack(MusicTrack::NONE);
        m_sounds.StopAllSounds();
    } else if (m_arena.TryCompleteLevel()) {
        if (!m_victoryRegistered) {
            const int coresBefore = m_saveData.totalEnergonCores;
            SaveSystem::RegisterLevelCompletion(m_saveData, m_arena.GetCurrentLevel(), m_arena.GetScore());
            m_completionCoresEarned = m_saveData.totalEnergonCores - coresBefore;
            m_victoryRegistered = true;
        }
        m_state = GameState::VICTORY;
        m_missionCompletionUI.ResetSelection();
        m_sounds.PlayMusicTrack(MusicTrack::NONE);
        m_sounds.StopAllSounds();
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground({ 10, 8, 24, 255 });

    // Opening Cinematic Scene
    if (m_state == GameState::OPENING_CINEMATIC) {
        DrawRectangleGradientV(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT,
            { 8, 10, 20, 255 }, { 22, 14, 38, 255 });

        float t = m_cinematicTimer;
        for (int i = 0; i < 40; ++i) {
            float starX = fmodf((float)i * 47.0f + t * 25.0f, (float)Constants::SCREEN_WIDTH);
            float starY = fmodf((float)i * 83.0f, (float)Constants::SCREEN_HEIGHT);
            float twinkle = (sinf(t * 3.0f + (float)i) + 1.0f) * 0.5f;
            DrawCircle((int)starX, (int)starY, 1.5f, ColorAlpha(Constants::ENERGON_CYAN, 0.4f + 0.6f * twinkle));
        }

        float panX = (float)Constants::SCREEN_WIDTH * 0.5f + sinf(t * 0.4f) * 60.0f;
        float panY = (float)Constants::SCREEN_HEIGHT * 0.42f;

        DrawCircle((int)panX, (int)panY, 130.0f + sinf(t * 0.8f) * 6.0f, { 18, 24, 44, 255 });
        DrawCircleLines((int)panX, (int)panY, 130.0f, Constants::ENERGON_CYAN);
        DrawCircleLines((int)panX, (int)panY, 80.0f, { 0, 140, 220, 160 });

        const char* crawlLine1 = "CYBERTRON HAS FALLEN TO CIVIL WAR.";
        const char* crawlLine2 = "MEGATRON'S DECEPTICONS WREAK HAVOC ACROSS ALL SECTORS.";
        const char* crawlLine3 = "ONE LEADER STANDS TO DEFEND THE FREEDOM OF ALL SENTIENT BEINGS.";

        float alpha1 = std::clamp((t - 1.0f) / 1.5f, 0.0f, 1.0f);
        float alpha2 = std::clamp((t - 4.0f) / 1.5f, 0.0f, 1.0f);
        float alpha3 = std::clamp((t - 7.5f) / 1.5f, 0.0f, 1.0f);

        if (alpha1 > 0.0f) {
            int w = MeasureText(crawlLine1, 20);
            DrawText(crawlLine1, (Constants::SCREEN_WIDTH - w) / 2, (int)panY + 160, 20, ColorAlpha(RAYWHITE, alpha1));
        }
        if (alpha2 > 0.0f) {
            int w = MeasureText(crawlLine2, 18);
            DrawText(crawlLine2, (Constants::SCREEN_WIDTH - w) / 2, (int)panY + 195, 18, ColorAlpha(Constants::AUTOBOT_RED, alpha2));
        }
        if (alpha3 > 0.0f) {
            int w = MeasureText(crawlLine3, 18);
            DrawText(crawlLine3, (Constants::SCREEN_WIDTH - w) / 2, (int)panY + 230, 18, ColorAlpha(Constants::ENERGON_CYAN, alpha3));
        }

        if (t > 10.0f) {
            float titleAlpha = std::clamp((t - 10.0f) / 1.5f, 0.0f, 1.0f);
            const char* mainTitle = "TRANSFORMERS: RISE OF CYBERTRON";
            int mw = MeasureText(mainTitle, 36);
            DrawText(mainTitle, (Constants::SCREEN_WIDTH - mw) / 2, 70, 36, ColorAlpha(Constants::AUTOBOT_RED, titleAlpha));
        }

        Rectangle skipBtn = { (float)Constants::SCREEN_WIDTH - 245.0f, 25.0f, 220.0f, 36.0f };
        DrawRectangleRounded(skipBtn, 0.25f, 4, Color{ 20, 35, 55, 220 });
        DrawRectangleRoundedLinesEx(skipBtn, 0.25f, 4, 1.5f, Constants::ENERGON_CYAN);
        const char* skipText = "[SPACE / ENTER / ESC] SKIP";
        int stw = MeasureText(skipText, 12);
        DrawText(skipText, (int)(skipBtn.x + (skipBtn.width - stw) / 2), (int)skipBtn.y + 12, 12, RAYWHITE);

        EndDrawing();
        return;
    }

    if (m_state == GameState::MENU) {
        m_menu.Draw(m_state, m_saveData, 0, 0, 0);
        EndDrawing();
        return;
    }

    if (m_state == GameState::LEVEL_SELECT) {
        m_levelSelectUI.Draw(m_saveData);
        EndDrawing();
        return;
    }

    if (m_state == GameState::SETTINGS) {
        m_settingsUI.Draw(m_saveData.settings);
        EndDrawing();
        return;
    }

    // World-Space & Active Level Rendering
    m_bg.Draw(m_camera.GetCamera());

    BeginMode2D(m_camera.GetCamera());
    m_bg.DrawGroundPlatform(m_camera.GetCamera());
    m_particles.Draw();

    for (const auto& enemy : m_arena.GetEnemies()) {
        enemy.Draw();
    }

    if (m_arena.GetBoss()) {
        m_arena.GetBoss()->Draw();
    }

    for (const auto& proj : m_projectiles) {
        proj.Draw();
    }

    m_player.Draw();
    EndMode2D();

    m_hud.Draw(m_player, m_arena, m_arena.GetBoss());

    if (m_state == GameState::PAUSED || m_state == GameState::GAME_OVER) {
        m_menu.Draw(m_state, m_saveData, m_arena.GetScore(), m_arena.GetKills(), m_arena.GetCurrentWave());
    } else if (m_state == GameState::VICTORY) {
        m_missionCompletionUI.Draw(m_saveData, m_arena.GetCurrentLevel(), m_arena.GetScore(),
                                   m_arena.GetKills(), m_completionCoresEarned, m_levelElapsedTime);
    }

    EndDrawing();
}

void Game::Run() {
    Init();

    while (!WindowShouldClose() && !m_shouldQuit) {
        float dt = GetFrameTime();
        if (dt > 0.05f) dt = 0.05f;

        Update(dt);
        Draw();
    }
}
