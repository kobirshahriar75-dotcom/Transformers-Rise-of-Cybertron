#include "Projectile.hpp"
#include <cmath>

Projectile::Projectile(int id, Faction faction, Vector2 pos, Vector2 vel, ProjectileType type, float damage, float lifeTime)
    : Entity(id, faction, pos, 20.0f, 20.0f, 1.0f)
    , m_type(type)
    , m_damage(damage)
    , m_lifeTimer(lifeTime)
    , m_maxLife(lifeTime)
    , m_trailTimer(0.0f)
{
    m_vel = vel;
    m_facing = (vel.x >= 0.0f) ? 1.0f : -1.0f;

    if (type == ProjectileType::PLAYER_ENERGON_WAVE) {
        m_width = 32.0f;
        m_height = 80.0f;
    } else if (type == ProjectileType::BOSS_FUSION_ORB) {
        m_width = 44.0f;
        m_height = 44.0f;
    } else if (type == ProjectileType::SHOCKWAVE_HYPER_BEAM) {
        m_width = 75.0f;
        m_height = 24.0f;
    } else if (type == ProjectileType::SOUNDWAVE_SONIC_DISC) {
        m_width = 36.0f;
        m_height = 36.0f;
    } else if (type == ProjectileType::STARSCREAM_NULL_RAY) {
        m_width = 45.0f;
        m_height = 14.0f;
    } else if (type == ProjectileType::STARSCREAM_MISSILE) {
        m_width = 28.0f;
        m_height = 12.0f;
    } else if (type == ProjectileType::PLAYER_PLASMA_BOLT) {
        m_width = 26.0f;
        m_height = 12.0f;
    } else {
        m_width = 18.0f;
        m_height = 10.0f;
    }
}

void Projectile::Update(float dt) {
    m_pos.x += m_vel.x * dt;
    m_pos.y += m_vel.y * dt;

    m_lifeTimer -= dt;
    m_trailTimer += dt;

    if (m_pos.x < Constants::ARENA_MIN_X - 60.0f || m_pos.x > Constants::ARENA_MAX_X + 60.0f) {
        m_isDead = true;
    }
}

Hitbox Projectile::GetHitbox() const {
    Hitbox hb;
    hb.rect = { m_pos.x - m_width * 0.5f, m_pos.y - m_height * 0.5f, m_width, m_height };
    hb.faction = m_faction;
    hb.damage = m_damage;

    float kbX = m_facing * 220.0f;
    if (m_type == ProjectileType::BOSS_FUSION_ORB) kbX = m_facing * 420.0f;
    else if (m_type == ProjectileType::SHOCKWAVE_HYPER_BEAM) kbX = m_facing * 380.0f;
    else if (m_type == ProjectileType::STARSCREAM_MISSILE) kbX = m_facing * 350.0f;

    hb.knockback = { kbX, -100.0f };
    hb.isCrit = (m_type == ProjectileType::PLAYER_ENERGON_WAVE || m_type == ProjectileType::SHOCKWAVE_HYPER_BEAM);
    hb.active = !m_isDead;
    hb.attackId = m_id;
    hb.hitStopDuration = (m_type == ProjectileType::BOSS_FUSION_ORB || m_type == ProjectileType::SHOCKWAVE_HYPER_BEAM) ? 0.08f : 0.04f;
    hb.causesScreenShake = (m_type == ProjectileType::BOSS_FUSION_ORB || m_type == ProjectileType::PLAYER_ENERGON_WAVE || m_type == ProjectileType::SHOCKWAVE_HYPER_BEAM);
    return hb;
}

void Projectile::Draw() const {
    if (m_isDead) return;

    if (m_type == ProjectileType::PLAYER_PLASMA_BOLT) {
        // Bright cyan plasma bolt with solid white core
        DrawRectangleRounded({ m_pos.x - 14.0f, m_pos.y - 6.0f, 28.0f, 12.0f }, 0.5f, 4, Constants::ENERGON_CYAN);
        DrawRectangleRounded({ m_pos.x - 10.0f, m_pos.y - 3.0f, 20.0f, 6.0f }, 0.5f, 4, RAYWHITE);
        DrawCircle((int)m_pos.x, (int)m_pos.y, 10.0f, { 0, 235, 255, 90 });
    }
    else if (m_type == ProjectileType::PLAYER_ENERGON_WAVE) {
        Color waveCol = Constants::ENERGON_ORANGE;
        DrawEllipse((int)m_pos.x, (int)m_pos.y, 16.0f, 42.0f, waveCol);
        DrawEllipse((int)m_pos.x + (int)(m_facing * 4.0f), (int)m_pos.y, 8.0f, 32.0f, RAYWHITE);
        DrawEllipseLines((int)m_pos.x, (int)m_pos.y, 18.0f, 45.0f, { 255, 220, 100, 220 });
    }
    else if (m_type == ProjectileType::BOSS_FUSION_ORB) {
        // Megatron's Dark Violet Fusion Orb
        float pulse = (sinf(m_trailTimer * 14.0f) + 1.0f) * 0.5f;
        Color deepPurple = { 160, 20, 220, 255 };
        DrawCircle((int)m_pos.x, (int)m_pos.y, 22.0f + pulse * 4.0f, { 180, 30, 240, 120 });
        DrawCircle((int)m_pos.x, (int)m_pos.y, 16.0f, deepPurple);
        DrawCircle((int)m_pos.x, (int)m_pos.y, 9.0f, RAYWHITE);
        DrawCircleLines((int)m_pos.x, (int)m_pos.y, 20.0f, { 255, 120, 255, 220 });
    }
    else if (m_type == ProjectileType::SHOCKWAVE_HYPER_BEAM) {
        // Shockwave's Hyper-Flux Concentrated Violet Beam
        float bx = m_pos.x - 36.0f;
        float by = m_pos.y - 12.0f;
        DrawRectangleRounded({ bx, by, 72.0f, 24.0f }, 0.4f, 4, { 170, 20, 230, 255 });
        DrawRectangleRounded({ bx + 6.0f, by + 4.0f, 60.0f, 16.0f }, 0.4f, 4, { 255, 225, 40, 255 }); // Gold core
        DrawRectangleRounded({ bx + 12.0f, by + 7.0f, 48.0f, 10.0f }, 0.4f, 4, RAYWHITE);
    }
    else if (m_type == ProjectileType::SOUNDWAVE_SONIC_DISC) {
        // Soundwave's Sonic Wave Ring
        float pulse = sinf(m_trailTimer * 20.0f) * 3.0f;
        DrawCircleLines((int)m_pos.x, (int)m_pos.y, 16.0f + pulse, { 80, 220, 255, 255 });
        DrawCircleLines((int)m_pos.x, (int)m_pos.y, 12.0f + pulse, RAYWHITE);
        DrawCircle((int)m_pos.x, (int)m_pos.y, 6.0f, Constants::AUTOBOT_BLUE);
    }
    else if (m_type == ProjectileType::STARSCREAM_NULL_RAY) {
        // Starscream's Crimson Null-Ray Bolt
        float rx = m_pos.x - 22.0f;
        float ry = m_pos.y - 7.0f;
        DrawRectangleRounded({ rx, ry, 44.0f, 14.0f }, 0.4f, 4, { 255, 30, 50, 255 });
        DrawRectangleRounded({ rx + 4.0f, ry + 3.0f, 36.0f, 8.0f }, 0.4f, 4, RAYWHITE);
    }
    else if (m_type == ProjectileType::STARSCREAM_MISSILE) {
        // Starscream's Seeker Missile
        float mx = m_pos.x - 14.0f;
        float my = m_pos.y - 6.0f;
        DrawRectangle((int)mx, (int)my, 28, 12, { 210, 215, 225, 255 });
        DrawTriangle(
            { mx + (m_facing > 0 ? 28.0f : 0.0f), my - 2.0f },
            { mx + (m_facing > 0 ? 36.0f : -8.0f), my + 6.0f },
            { mx + (m_facing > 0 ? 28.0f : 0.0f), my + 14.0f },
            Constants::AUTOBOT_RED
        );
        // Rocket flame
        float flameX = (m_facing > 0) ? mx : (mx + 28.0f);
        DrawCircle((int)flameX, (int)(my + 6.0f), 5.0f, { 255, 140, 20, 255 });
    }
    else if (m_type == ProjectileType::ENEMY_LASER) {
        DrawRectangleRounded({ m_pos.x - 10.0f, m_pos.y - 4.0f, 20.0f, 8.0f }, 0.4f, 4, Constants::DECEPTICON_PURPLE);
        DrawRectangle((int)m_pos.x - 6, (int)m_pos.y - 2, 12, 4, RAYWHITE);
    }
}
