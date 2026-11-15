#pragma once

#include "Entity.hpp"

enum class ProjectileType {
    PLAYER_PLASMA_BOLT,
    PLAYER_ENERGON_WAVE,
    ENEMY_LASER,
    BOSS_FUSION_ORB,
    SHOCKWAVE_HYPER_BEAM,
    SOUNDWAVE_SONIC_DISC,
    STARSCREAM_NULL_RAY,
    STARSCREAM_MISSILE
};

class Projectile : public Entity {
public:
    Projectile(int id, Faction faction, Vector2 pos, Vector2 vel, ProjectileType type, float damage, float lifeTime);

    void Update(float dt) override;
    void Draw() const override;

    Hitbox GetHitbox() const;
    ProjectileType GetType() const { return m_type; }
    bool HasExpired() const { return m_lifeTimer <= 0.0f || m_isDead; }

private:
    ProjectileType m_type;
    float m_damage;
    float m_lifeTimer;
    float m_maxLife;
    float m_trailTimer;
};
