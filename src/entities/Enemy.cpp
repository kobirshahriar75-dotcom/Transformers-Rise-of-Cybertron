#include "Enemy.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>

Enemy::Enemy(int id, EnemyType type, Vector2 pos,
             float aerialProjectileDamage,
             float aerialAttackInterval,
             float aerialProjectileSpeed)
    : Entity(
        id,
        Faction::ENEMY,
        pos,
        (type == EnemyType::BRUTE ? 68.0f : (type == EnemyType::SEEKER_AERIAL ? 52.0f : 42.0f)),
        (type == EnemyType::BRUTE ? 104.0f : (type == EnemyType::SEEKER_AERIAL ? 70.0f : 72.0f)),
        (type == EnemyType::BRUTE ? 130.0f : (type == EnemyType::SEEKER_AERIAL ? 55.0f : 45.0f))
    )
    , m_type(type)
    , m_state(EnemyState::PATROL)
    , m_stateTimer(0.0f)
    , m_attackCooldown(1.0f)
    , m_animTimer(0.0f)
    , m_patrolTimer(2.0f)
    , m_attackId(1)
    , m_aerialProjectileDamage(aerialProjectileDamage)
    , m_aerialAttackInterval(aerialAttackInterval)
    , m_aerialProjectileSpeed(aerialProjectileSpeed)
{
    m_facing = -1.0f;
}

void Enemy::Update(float dt) {
    m_animTimer += dt;

    if (m_hurtTimer > 0.0f) m_hurtTimer -= dt;
    if (m_flashTimer > 0.0f) m_flashTimer -= dt;
    if (m_invulnTimer > 0.0f) m_invulnTimer -= dt;
    if (m_attackCooldown > 0.0f) m_attackCooldown -= dt;

    if (m_type == EnemyType::SEEKER_AERIAL) {
        m_pos.x += m_vel.x * dt;
        m_pos.y += m_vel.y * dt;
        m_isGrounded = false;

        if (m_pos.y < 120.0f) m_pos.y = 120.0f;
        if (m_pos.y > Constants::GROUND_Y - 40.0f) m_pos.y = Constants::GROUND_Y - 40.0f;

        if (m_pos.x - m_width * 0.5f < Constants::ARENA_MIN_X) {
            m_pos.x = Constants::ARENA_MIN_X + m_width * 0.5f;
            m_vel.x = 0.0f;
        } else if (m_pos.x + m_width * 0.5f > Constants::ARENA_MAX_X) {
            m_pos.x = Constants::ARENA_MAX_X - m_width * 0.5f;
            m_vel.x = 0.0f;
        }
        return;
    }

    // Ground Detection & Vertical Physics
    if (m_pos.y < Constants::GROUND_Y) {
        m_isGrounded = false;
    } else {
        m_pos.y = Constants::GROUND_Y;
        if (m_vel.y > 0.0f) {
            m_vel.y = 0.0f;
        }
        m_isGrounded = true;
    }

    if (!m_isGrounded || m_vel.y < 0.0f) {
        m_vel.y += Constants::GRAVITY * dt;
        m_isGrounded = false;
    }

    if (m_vel.y < -850.0f) m_vel.y = -850.0f;
    if (m_vel.y > 1200.0f) m_vel.y = 1200.0f;

    m_pos.x += m_vel.x * dt;
    m_pos.y += m_vel.y * dt;

    float friction = (m_type == EnemyType::BRUTE) ? 1400.0f : 900.0f;
    if (m_vel.x > 0.0f) {
        m_vel.x = std::max(0.0f, m_vel.x - friction * dt);
    } else if (m_vel.x < 0.0f) {
        m_vel.x = std::min(0.0f, m_vel.x + friction * dt);
    }

    if (m_pos.y >= Constants::GROUND_Y) {
        m_pos.y = Constants::GROUND_Y;
        if (m_vel.y > 0.0f) {
            m_vel.y = 0.0f;
        }
        m_isGrounded = true;
    }

    if (m_pos.x - m_width * 0.5f < Constants::ARENA_MIN_X) {
        m_pos.x = Constants::ARENA_MIN_X + m_width * 0.5f;
        m_vel.x = 0.0f;
    } else if (m_pos.x + m_width * 0.5f > Constants::ARENA_MAX_X) {
        m_pos.x = Constants::ARENA_MAX_X - m_width * 0.5f;
        m_vel.x = 0.0f;
    }
}

void Enemy::UpdateAI(Vector2 playerPos, float dt, SoundSystem& sounds, ParticleSystem& particles, std::vector<Projectile>& outProjectiles) {
    if (m_isDead) return;

    float distToPlayer = fabsf(playerPos.x - m_pos.x);
    float dirToPlayer = (playerPos.x > m_pos.x) ? 1.0f : -1.0f;

    // Aerial Seeker AI
    if (m_type == EnemyType::SEEKER_AERIAL) {
        m_facing = dirToPlayer;
        float flySpeed = 190.0f;

        // Hover around player's altitude
        float targetY = playerPos.y - 60.0f + sinf(m_animTimer * 3.0f) * 40.0f;
        m_vel.y = (targetY - m_pos.y) * 1.8f;

        if (distToPlayer > 300.0f) {
            m_vel.x = m_facing * flySpeed;
        } else if (distToPlayer < 180.0f) {
            m_vel.x = -m_facing * (flySpeed * 0.8f);
        } else {
            m_vel.x = 0.0f;
        }

        if (m_attackCooldown <= 0.0f && distToPlayer < 650.0f) {
            m_attackCooldown = m_aerialAttackInterval;
            sounds.Play(SoundType::BLASTER, 1.2f, 0.8f);

            Vector2 laserMuzzle = { m_pos.x + m_facing * 28.0f, m_pos.y - 35.0f };
            Vector2 laserVel = { m_facing * m_aerialProjectileSpeed, 0.0f };
            static int seekerProjId = 7000;
            outProjectiles.emplace_back(seekerProjId++, Faction::ENEMY, laserMuzzle, laserVel, ProjectileType::ENEMY_LASER, m_aerialProjectileDamage, 2.5f);
            particles.EmitSparks(laserMuzzle, 4, Constants::DECEPTICON_PURPLE, 60.0f, 160.0f);
        }
        return;
    }

    // Ground AI
    switch (m_state) {
        case EnemyState::PATROL:
            m_patrolTimer -= dt;
            m_vel.x = m_facing * 80.0f;
            if (m_patrolTimer <= 0.0f) {
                m_facing = -m_facing;
                m_patrolTimer = 2.0f + (float)(rand() % 3);
            }
            if (distToPlayer < 650.0f) {
                m_state = EnemyState::PURSUE;
            }
            break;

        case EnemyState::PURSUE: {
            m_facing = dirToPlayer;
            float moveSpeed = (m_type == EnemyType::BRUTE) ? 120.0f : 210.0f;
            m_vel.x = m_facing * moveSpeed;

            float attackRange = (m_type == EnemyType::BRUTE) ? 80.0f : 55.0f;
            if (distToPlayer <= attackRange && m_attackCooldown <= 0.0f) {
                m_state = EnemyState::WINDUP;
                m_stateTimer = (m_type == EnemyType::BRUTE) ? 0.45f : 0.28f;
                m_vel.x = 0.0f;
            } else if (distToPlayer > 280.0f && distToPlayer < 520.0f && m_type == EnemyType::DRONE && m_attackCooldown <= 0.0f) {
                // Drone ranged shot
                m_attackCooldown = 2.5f;
                sounds.Play(SoundType::BLASTER, 1.3f, 0.7f);
                Vector2 muzzle = { m_pos.x + m_facing * 24.0f, m_pos.y - 45.0f };
                Vector2 projVel = { m_facing * 600.0f, 0.0f };
                static int droneProjId = 5000;
                outProjectiles.emplace_back(droneProjId++, Faction::ENEMY, muzzle, projVel, ProjectileType::ENEMY_LASER, 12.0f, 2.0f);
                particles.EmitSparks(muzzle, 4, Constants::DECEPTICON_PURPLE, 60.0f, 180.0f);
            }
            break;
        }

        case EnemyState::WINDUP:
            m_stateTimer -= dt;
            m_facing = dirToPlayer;
            if (m_stateTimer <= 0.0f) {
                m_state = EnemyState::ATTACK;
                m_stateTimer = 0.18f;
                m_attackId++;
                if (m_type == EnemyType::BRUTE) {
                    sounds.Play(SoundType::TRUCK_RAM, 0.8f, 0.9f);
                    particles.EmitShockwave({ m_pos.x + m_facing * 40.0f, Constants::GROUND_Y - 4.0f }, 45.0f, Constants::DECEPTICON_PURPLE, 0.2f, 3.0f);
                } else {
                    sounds.Play(SoundType::SLASH_LIGHT, 0.9f, 0.8f);
                }
            }
            break;

        case EnemyState::ATTACK:
            m_stateTimer -= dt;
            if (m_stateTimer <= 0.0f) {
                m_state = EnemyState::RECOVER;
                m_stateTimer = (m_type == EnemyType::BRUTE) ? 0.65f : 0.35f;
                m_attackCooldown = 1.2f;
            }
            break;

        case EnemyState::RECOVER:
            m_stateTimer -= dt;
            m_vel.x = 0.0f;
            if (m_stateTimer <= 0.0f) {
                m_state = EnemyState::PURSUE;
            }
            break;

        default:
            break;
    }
}

Hitbox Enemy::GetMeleeHitbox() const {
    Hitbox hb;
    hb.active = false;
    if (m_state != EnemyState::ATTACK) return hb;

    float reach = (m_type == EnemyType::BRUTE) ? 65.0f : 45.0f;
    float reachH = (m_type == EnemyType::BRUTE) ? 75.0f : 50.0f;
    float hitX = (m_facing > 0.0f) ? m_pos.x : (m_pos.x - reach);
    float hitY = m_pos.y - reachH;

    hb.rect = { hitX, hitY, reach, reachH };
    hb.faction = Faction::ENEMY;
    hb.damage = (m_type == EnemyType::BRUTE) ? 22.0f : 12.0f;
    hb.knockback = { m_facing * ((m_type == EnemyType::BRUTE) ? 350.0f : 200.0f), (m_type == EnemyType::BRUTE) ? -180.0f : -80.0f };
    hb.isCrit = (m_type == EnemyType::BRUTE);
    hb.active = true;
    hb.attackId = m_attackId;
    hb.hitStopDuration = (m_type == EnemyType::BRUTE) ? Constants::HIT_STOP_HEAVY : Constants::HIT_STOP_LIGHT;
    hb.causesScreenShake = (m_type == EnemyType::BRUTE);
    return hb;
}

void Enemy::DrawDrone() const {
    float f = m_facing;
    Vector2 p = m_pos;

    if (m_flashTimer > 0.0f && ((int)(m_flashTimer * 30.0f) % 2 == 0)) {
        DrawRectangleRounded({ p.x - 21.0f, p.y - 72.0f, 42.0f, 72.0f }, 0.2f, 4, RAYWHITE);
        return;
    }

    Color darkPurple = Constants::DECEPTICON_DARK;
    Color purple = Constants::DECEPTICON_PURPLE;

    // Legs
    float foot1X = (f > 0.0f) ? (p.x - 16.0f) : (p.x - 20.0f);
    float foot2X = (f > 0.0f) ? (p.x + 2.0f) : (p.x - 2.0f);
    DrawRectangle((int)foot1X, (int)(p.y - 32.0f), 10, 32, darkPurple);
    DrawRectangle((int)foot2X, (int)(p.y - 32.0f), 10, 32, darkPurple);

    // Torso
    DrawRectangleRounded({ p.x - 18.0f, p.y - 60.0f, 36.0f, 30.0f }, 0.2f, 4, darkPurple);
    DrawRectangleRoundedLinesEx({ p.x - 18.0f, p.y - 60.0f, 36.0f, 30.0f }, 0.2f, 4, 1.5f, purple);

    // Head
    DrawRectangle((int)(p.x - 8.0f), (int)(p.y - 74.0f), 16, 14, darkPurple);
    DrawRectangle((int)(p.x - 5.0f + f * 2.0f), (int)(p.y - 69.0f), 8, 3, { 255, 30, 40, 255 }); // Red eye

    // Arm weapon
    float armW = 28.0f;
    float armMinX = (f > 0.0f) ? (p.x + 12.0f) : (p.x - 12.0f - armW);
    DrawRectangle((int)armMinX, (int)(p.y - 48.0f), (int)armW, 8, purple);
}

void Enemy::DrawBrute() const {
    float f = m_facing;
    Vector2 p = m_pos;

    if (m_flashTimer > 0.0f && ((int)(m_flashTimer * 30.0f) % 2 == 0)) {
        DrawRectangleRounded({ p.x - 34.0f, p.y - 104.0f, 68.0f, 104.0f }, 0.2f, 4, RAYWHITE);
        return;
    }

    Color darkSteel = { 38, 32, 48, 255 };
    Color purple = Constants::DECEPTICON_PURPLE;
    Color chrome = { 160, 170, 185, 255 };

    // Heavy legs
    DrawRectangle((int)(p.x - 24.0f), (int)(p.y - 48.0f), 18, 48, darkSteel);
    DrawRectangle((int)(p.x + 6.0f), (int)(p.y - 48.0f), 18, 48, darkSteel);

    float foot1X = (f > 0.0f) ? (p.x - 28.0f) : (p.x - 32.0f);
    float foot2X = (f > 0.0f) ? (p.x + 2.0f) : (p.x - 4.0f);
    DrawRectangle((int)foot1X, (int)(p.y - 14.0f), 24, 14, chrome);
    DrawRectangle((int)foot2X, (int)(p.y - 14.0f), 24, 14, chrome);

    // Heavy Torso
    DrawRectangleRounded({ p.x - 30.0f, p.y - 92.0f, 60.0f, 46.0f }, 0.2f, 4, darkSteel);
    DrawRectangleRoundedLinesEx({ p.x - 30.0f, p.y - 92.0f, 60.0f, 46.0f }, 0.2f, 4, 2.0f, purple);

    // Head
    DrawRectangle((int)(p.x - 12.0f), (int)(p.y - 106.0f), 24, 16, darkSteel);
    DrawRectangle((int)(p.x - 7.0f + f * 3.0f), (int)(p.y - 100.0f), 10, 4, { 255, 30, 40, 255 });

    // Heavy Arm Gauntlet
    float gauntW = 36.0f;
    float gauntMinX = (f > 0.0f) ? (p.x + 16.0f) : (p.x - 16.0f - gauntW);
    DrawRectangle((int)gauntMinX, (int)(p.y - 75.0f), (int)gauntW, 20, chrome);
}

void Enemy::DrawSeeker() const {
    float f = m_facing;
    Vector2 p = m_pos;

    if (m_flashTimer > 0.0f && ((int)(m_flashTimer * 30.0f) % 2 == 0)) {
        DrawRectangleRounded({ p.x - 26.0f, p.y - 70.0f, 52.0f, 70.0f }, 0.2f, 4, RAYWHITE);
        return;
    }

    Color darkPurple = { 65, 25, 95, 255 };
    Color purple = Constants::DECEPTICON_PURPLE;
    Color silver = { 190, 200, 215, 255 };

    // Swept Back Wings
    Vector2 wRoot = { p.x - f * 8.0f, p.y - 45.0f };
    Vector2 wTip1 = { p.x - f * 38.0f, p.y - 75.0f };
    Vector2 wTip2 = { p.x - f * 30.0f, p.y - 25.0f };
    DrawTriangle(wRoot, (f > 0.0f ? wTip1 : wTip2), (f > 0.0f ? wTip2 : wTip1), darkPurple);

    // Body
    DrawRectangleRounded({ p.x - 18.0f, p.y - 58.0f, 36.0f, 34.0f }, 0.2f, 4, darkPurple);
    DrawRectangleRoundedLinesEx({ p.x - 18.0f, p.y - 58.0f, 36.0f, 34.0f }, 0.2f, 4, 1.5f, purple);

    // Head
    DrawRectangle((int)(p.x - 8.0f), (int)(p.y - 70.0f), 16, 12, darkPurple);
    DrawRectangle((int)(p.x - 5.0f + f * 2.0f), (int)(p.y - 66.0f), 8, 3, { 255, 20, 30, 255 });

    // Arm Blaster
    float blW = 28.0f;
    float blMinX = (f > 0.0f) ? (p.x + 10.0f) : (p.x - 10.0f - blW);
    DrawRectangle((int)blMinX, (int)(p.y - 44.0f), (int)blW, 8, silver);

    // Jet Exhaust Flame
    float flameX = p.x - f * 18.0f;
    float flicker = sinf(m_animTimer * 20.0f) * 3.0f;
    DrawCircle((int)flameX, (int)(p.y - 22.0f), 5.0f + flicker, { 255, 140, 20, 255 });
}

void Enemy::Draw() const {
    if (m_isDead) return;

    if (m_type == EnemyType::BRUTE) {
        DrawBrute();
    } else if (m_type == EnemyType::SEEKER_AERIAL) {
        DrawSeeker();
    } else {
        DrawDrone();
    }
}
