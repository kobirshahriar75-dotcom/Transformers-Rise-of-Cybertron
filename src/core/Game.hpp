#pragma once

#include "raylib.h"
#include "Constants.hpp"
#include "CameraManager.hpp"
#include "SoundSystem.hpp"
#include "SaveSystem.hpp"
#include "Settings.hpp"
#include "../world/ParallaxBackground.hpp"
#include "../world/Arena.hpp"
#include "../entities/Player.hpp"
#include "../entities/Projectile.hpp"
#include "../combat/CombatSystem.hpp"
#include "../effects/ParticleSystem.hpp"
#include "../ui/HUD.hpp"
#include "../ui/MenuSystem.hpp"
#include "../ui/LevelSelectUI.hpp"
#include "../ui/SettingsUI.hpp"
#include "../ui/MissionCompletionUI.hpp"
#include <vector>

class Game {
public:
    Game();
    ~Game();

    void Run();

private:
    void Init();
    void Reset();
    bool StartLevel(int levelNumber);
    void Update(float dt);
    void Draw();
    void Shutdown();

    void ApplySettings();

    GameState m_state;
    SaveData m_saveData;
    CameraManager m_camera;
    SoundSystem m_sounds;
    ParallaxBackground m_bg;
    Arena m_arena;
    Player m_player;
    std::vector<Projectile> m_projectiles;
    CombatSystem m_combat;
    ParticleSystem m_particles;
    HUD m_hud;
    MenuSystem m_menu;
    LevelSelectUI m_levelSelectUI;
    SettingsUI m_settingsUI;
    MissionCompletionUI m_missionCompletionUI;

    bool m_shouldQuit;
    bool m_victoryRegistered;
    float m_cinematicTimer;
    float m_levelElapsedTime;
    int m_completionCoresEarned;
};
