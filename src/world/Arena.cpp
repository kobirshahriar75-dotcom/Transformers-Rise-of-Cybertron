#include "Arena.hpp"
#include "../core/Constants.hpp"

Arena::Arena()
    : m_levelNumber(1)
    , m_currentWave(0)
    , m_kills(0)
    , m_score(0)
    , m_isVictory(false)
    , m_waveCooldown(1.5f)
    , m_bannerTimer(0.0f)
    , m_boss(nullptr)
{
}

void Arena::Init() {
    Reset();
}

void Arena::SetCurrentLevel(int levelNumber) {
    if (levelNumber < 1) levelNumber = 1;
    if (levelNumber > 10) levelNumber = 10;
    m_levelNumber = levelNumber;
    Reset();
}

void Arena::Reset() {
    m_currentWave = 0;
    m_kills = 0;
    m_score = 0;
    m_isVictory = false;
    m_waveCooldown = 1.0f;
    m_bannerTimer = 0.0f;
    m_enemies.clear();
    m_boss.reset();
}

int Arena::GetTotalWaves() const {
    return GetLevelInfo().totalWaves;
}

bool Arena::IsWaveClear() const {
    for (const auto& enemy : m_enemies) {
        if (!enemy.IsDead()) return false;
    }

    // The boss is an additional final-wave objective, not a replacement for
    // checking any other enemies that may be present in that wave.
    if (m_currentWave == GetTotalWaves() && GetLevelInfo().bossType != LevelBossType::NONE) {
        return m_boss != nullptr && m_boss->IsDead();
    }
    return true;
}

bool Arena::TryCompleteLevel() {
    if (m_isVictory) return true;

    // Do not permit an empty arena, an intermediate wave, or an unspawned
    // boss encounter to be mistaken for a completed mission.
    if (m_currentWave <= 0 || m_currentWave != GetTotalWaves()) return false;
    if (!IsWaveClear()) return false;

    m_isVictory = true;
    m_waveBanner = "SECTOR SECURED: " + GetLevelInfo().name;
    m_bannerTimer = 5.0f;
    return true;
}

void Arena::SpawnWave(int waveNum, SoundSystem& sounds, ParticleSystem& particles) {
    m_currentWave = waveNum;
    m_bannerTimer = 3.5f;
    static int enemyIdGen = 200;

    const LevelInfo& level = GetLevelInfo();
    int totalWaves = level.totalWaves;

    // Check if this wave is the boss encounter
    if (waveNum == totalWaves && level.bossType != LevelBossType::NONE) {
        std::string bossName = "CRITICAL ALERT: ";
        if (level.bossType == LevelBossType::SHOCKWAVE) bossName += "SHOCKWAVE DETECTED!";
        else if (level.bossType == LevelBossType::SOUNDWAVE) bossName += "SOUNDWAVE INTERCEPT!";
        else if (level.bossType == LevelBossType::STARSCREAM) bossName += "STARSCREAM AERIAL COMBAT!";
        else if (level.bossType == LevelBossType::MEGATRON) bossName += "MEGATRON SHOWDOWN!";

        m_waveBanner = bossName;
        sounds.Play(SoundType::BOSS_ROAR, 1.0f, 1.0f);
        sounds.Play(SoundType::EXPLOSION, 0.8f, 1.0f);

        float bossY = (level.bossType == LevelBossType::STARSCREAM) ? 260.0f : Constants::GROUND_Y;
        m_boss = std::make_unique<Boss>(999, level.bossType, Vector2{ 2000.0f, bossY });

        particles.EmitShockwave({ 2000.0f, bossY - 4.0f }, 150.0f, Constants::ENERGON_PURPLE, 0.5f, 6.0f);
        particles.EmitSparks({ 2000.0f, bossY - 50.0f }, 30, Constants::ENERGON_PURPLE, 150.0f, 400.0f);
        return;
    }

    // Standard Wave Formulation
    m_waveBanner = "SECTOR " + std::to_string(m_levelNumber) + " - WAVE " + std::to_string(waveNum) + " / " + std::to_string(totalWaves);
    sounds.Play(SoundType::SLASH_LIGHT, 0.8f, 0.9f);

    bool isAerialLevel = (m_levelNumber == 7 || m_levelNumber == 8);

    if (isAerialLevel) {
        // Spawn Flying Seeker Drones in the air
        // Level 7 is the aerial-combat tutorial/transition mission: keep its
        // four waves readable and give the player room to learn flight.
        // Level 8 retains its original, more demanding seeker pressure.
        const bool levelSevenTraining = (m_levelNumber == 7);
        const int seekerCount = (levelSevenTraining ? 2 : 3) + waveNum;
        const float seekerDamage = levelSevenTraining ? 10.0f : 14.0f;
        const float seekerInterval = levelSevenTraining ? 3.0f : 2.2f;
        const float seekerSpeed = levelSevenTraining ? 520.0f : 650.0f;
        const float startX = 850.0f;
        const float stepX = 280.0f;
        for (int i = 0; i < seekerCount; ++i) {
            float x = startX + i * stepX;
            float y = 220.0f + (float)((i % 3) * 60);
            if (x > Constants::ARENA_MAX_X - 100.0f) x = Constants::ARENA_MAX_X - 100.0f - (i * 40.0f);
            m_enemies.emplace_back(enemyIdGen++, EnemyType::SEEKER_AERIAL,
                Vector2{ x, y }, seekerDamage, seekerInterval, seekerSpeed);
        }
    } else {
        int droneCount = 2 + (m_levelNumber / 3) + waveNum;
        int bruteCount = (m_levelNumber >= 2) ? (1 + (m_levelNumber / 4) + (waveNum > 1 ? 1 : 0)) : 0;

        float spawnStartX = 800.0f;
        float spawnStep = 320.0f;

        for (int i = 0; i < droneCount; ++i) {
            float x = spawnStartX + i * spawnStep;
            if (x > Constants::ARENA_MAX_X - 100.0f) x = Constants::ARENA_MAX_X - 100.0f - (i * 40.0f);
            m_enemies.emplace_back(enemyIdGen++, EnemyType::DRONE, Vector2{ x, Constants::GROUND_Y });
        }

        for (int j = 0; j < bruteCount; ++j) {
            float x = spawnStartX + 160.0f + j * (spawnStep + 80.0f);
            if (x > Constants::ARENA_MAX_X - 100.0f) x = Constants::ARENA_MAX_X - 150.0f - (j * 60.0f);
            m_enemies.emplace_back(enemyIdGen++, EnemyType::BRUTE, Vector2{ x, Constants::GROUND_Y });
        }
    }
}

void Arena::Update(float dt, Vector2 playerPos, SoundSystem& sounds, ParticleSystem& particles) {
    (void)playerPos;
    if (m_bannerTimer > 0.0f) {
        m_bannerTimer -= dt;
    }

    if (m_isVictory) return;

    int totalWaves = GetTotalWaves();

    if (m_currentWave == 0) {
        m_waveCooldown -= dt;
        if (m_waveCooldown <= 0.0f) {
            SpawnWave(1, sounds, particles);
        }
    } else if (IsWaveClear()) {
        if (m_currentWave < totalWaves) {
            m_waveCooldown -= dt;
            if (m_waveCooldown <= 0.0f) {
                m_waveCooldown = 2.0f;
                SpawnWave(m_currentWave + 1, sounds, particles);
            }
        } else if (m_currentWave == totalWaves) {
            // This handles normal levels and boss levels through one objective check.
            TryCompleteLevel();
        }
    }

    // Also evaluate the final objective here. Game calls this again after combat
    // resolution so the final enemy/boss defeat is recognized in the same frame.
    TryCompleteLevel();
}
