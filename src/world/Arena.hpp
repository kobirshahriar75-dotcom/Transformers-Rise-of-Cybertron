#pragma once

#include "raylib.h"
#include "../entities/Enemy.hpp"
#include "../entities/Boss.hpp"
#include "../core/SoundSystem.hpp"
#include "../effects/ParticleSystem.hpp"
#include "LevelData.hpp"
#include <vector>
#include <string>
#include <memory>

class Arena {
public:
    Arena();

    void Init();
    void Reset();
    void SetCurrentLevel(int levelNumber);
    int GetCurrentLevel() const { return m_levelNumber; }
    const LevelInfo& GetLevelInfo() const { return LevelDatabase::GetLevel(m_levelNumber); }

    void Update(float dt, Vector2 playerPos, SoundSystem& sounds, ParticleSystem& particles);

    int GetCurrentWave() const { return m_currentWave; }
    int GetTotalWaves() const;
    int GetKills() const { return m_kills; }
    int GetScore() const { return m_score; }
    void AddKill(int scoreReward) { m_kills++; m_score += scoreReward; }

    std::vector<Enemy>& GetEnemies() { return m_enemies; }
    Boss* GetBoss() { return m_boss.get(); }

    bool IsWaveClear() const;
    // Evaluates the final objective after combat resolution. Safe to call every frame.
    // A completed level is latched so the completion flow cannot be skipped or repeated.
    bool TryCompleteLevel();
    bool IsBossActive() const { return m_boss != nullptr && !m_boss->IsDead(); }
    bool IsVictory() const { return m_isVictory; }

    std::string GetWaveBanner() const { return m_waveBanner; }
    float GetBannerTimer() const { return m_bannerTimer; }

private:
    int m_levelNumber;
    int m_currentWave;
    int m_kills;
    int m_score;
    bool m_isVictory;
    float m_waveCooldown;
    float m_bannerTimer;
    std::string m_waveBanner;

    std::vector<Enemy> m_enemies;
    std::unique_ptr<Boss> m_boss;

    void SpawnWave(int waveNum, SoundSystem& sounds, ParticleSystem& particles);
};
