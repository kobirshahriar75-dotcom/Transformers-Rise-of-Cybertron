#pragma once

#include "raylib.h"
#include "Hitbox.hpp"
#include "../entities/Player.hpp"
#include "../entities/Enemy.hpp"
#include "../entities/Boss.hpp"
#include "../entities/Projectile.hpp"
#include "../effects/ParticleSystem.hpp"
#include "../core/CameraManager.hpp"
#include "../core/SoundSystem.hpp"
#include <vector>
#include <set>

class CombatSystem {
public:
    CombatSystem();

    void Update(
        float dt,
        Player& player,
        std::vector<Enemy>& enemies,
        Boss* boss,
        std::vector<Projectile>& projectiles,
        ParticleSystem& particles,
        CameraManager& camera,
        SoundSystem& sounds
    );

    float GetHitStopTimer() const { return m_hitStopTimer; }
    void TriggerHitStop(float duration) { m_hitStopTimer = duration; }
    void UpdateHitStop(float dt);
    void Reset();

private:
    float m_hitStopTimer;
    // Set of (attackId << 16 | targetEntityId) to prevent multi-hits from same strike
    std::set<uint32_t> m_processedHits;

    bool HasHit(int attackId, int targetId) const;
    void RecordHit(int attackId, int targetId);
};
