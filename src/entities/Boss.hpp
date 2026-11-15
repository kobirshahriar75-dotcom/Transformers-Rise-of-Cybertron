#pragma once

#include "Entity.hpp"
#include "Projectile.hpp"
#include "../core/SoundSystem.hpp"
#include "../effects/ParticleSystem.hpp"
#include "../world/LevelData.hpp"
#include <vector>

enum class BossState {
    INTRO,
    NEUTRAL,
    PURSUE,
    TELEGRAPH_ATTACK_1,
    EXECUTE_ATTACK_1,
    TELEGRAPH_ATTACK_2,
    EXECUTE_ATTACK_2,
    SPECIAL_ACTION,
    STAGGER,
    DEFEATED
};

class Boss : public Entity {
public:
    Boss(int id, LevelBossType type, Vector2 pos);

    void Update(float dt) override;
    void Draw() const override;

    void UpdateAI(Vector2 playerPos, float dt, SoundSystem& sounds, ParticleSystem& particles, std::vector<Projectile>& outProjectiles);
    Hitbox GetMeleeHitbox() const;
    Hitbox GetSpecialHitbox() const;
    Hitbox GetDashHitbox() const { return GetSpecialHitbox(); }

    LevelBossType GetBossType() const { return m_bossType; }
    bool IsInPhaseTwo() const { return m_hp <= m_maxHp * 0.5f; }
    BossState GetState() const { return m_state; }
    void TriggerDefeat();

private:
    LevelBossType m_bossType;
    BossState m_state;
    float m_stateTimer;
    float m_attackCooldown;
    float m_animTimer;
    int m_attackId;
    float m_auraTimer;
    bool m_phaseTwoAnnounced;
    float m_flightHoverY;

    // Boss-specific rendering methods matching reference images
    void DrawShockwave() const;
    void DrawSoundwave() const;
    void DrawStarscream() const;
    void DrawMegatron() const;

    // Accessory rendering helpers
    void DrawShockwaveArmCannon(Vector2 armPos) const;
    void DrawSoundwaveShoulderCannon(Vector2 shoulderPos) const;
    void DrawStarscreamWings(Vector2 center) const;
    void DrawMegatronFusionCannon(Vector2 armPos) const;
    void DrawMegatronEnergonMace(Vector2 handPos) const;
};
