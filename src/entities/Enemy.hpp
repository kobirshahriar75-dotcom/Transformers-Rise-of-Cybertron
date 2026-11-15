#pragma once

#include "Entity.hpp"
#include "Projectile.hpp"
#include "../core/SoundSystem.hpp"
#include "../effects/ParticleSystem.hpp"
#include <vector>

enum class EnemyState {
    IDLE,
    PATROL,
    PURSUE,
    WINDUP,
    ATTACK,
    RECOVER
};

class Enemy : public Entity {
public:
    Enemy(int id, EnemyType type, Vector2 pos,
          float aerialProjectileDamage = 14.0f,
          float aerialAttackInterval = 2.2f,
          float aerialProjectileSpeed = 650.0f);

    void Update(float dt) override;
    void Draw() const override;

    void UpdateAI(Vector2 playerPos, float dt, SoundSystem& sounds, ParticleSystem& particles, std::vector<Projectile>& outProjectiles);
    Hitbox GetMeleeHitbox() const;
    EnemyType GetType() const { return m_type; }

private:
    EnemyType m_type;
    EnemyState m_state;
    float m_stateTimer;
    float m_attackCooldown;
    float m_animTimer;
    float m_patrolTimer;
    int m_attackId;

    // Per-mission tuning for aerial seekers. Ground enemies ignore these.
    float m_aerialProjectileDamage;
    float m_aerialAttackInterval;
    float m_aerialProjectileSpeed;

    // Drawing
    void DrawDrone() const;
    void DrawBrute() const;
    void DrawSeeker() const;
};
