#include "Boss.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace {
    inline float RandF() {
        return (float)rand() / (float)RAND_MAX;
    }
}

Boss::Boss(int id, LevelBossType type, Vector2 pos)
    : Entity(id, Faction::ENEMY, pos, 68.0f, 130.0f, 500.0f)
    , m_bossType(type)
    , m_state(BossState::INTRO)
    , m_stateTimer(2.0f)
    , m_attackCooldown(1.5f)
    , m_animTimer(0.0f)
    , m_attackId(100)
    , m_auraTimer(0.0f)
    , m_phaseTwoAnnounced(false)
    , m_flightHoverY(260.0f)
{
    m_facing = -1.0f;

    if (type == LevelBossType::SHOCKWAVE) {
        m_maxHp = 550.0f;
        m_hp = 550.0f;
        m_width = 68.0f;
        m_height = 132.0f;
    } else if (type == LevelBossType::SOUNDWAVE) {
        m_maxHp = 500.0f;
        m_hp = 500.0f;
        m_width = 64.0f;
        m_height = 128.0f;
    } else if (type == LevelBossType::STARSCREAM) {
        m_maxHp = 480.0f;
        m_hp = 480.0f;
        m_width = 72.0f;
        m_height = 126.0f;
        m_pos.y = m_flightHoverY; // Starscream starts in flight
    } else { // MEGATRON
        m_maxHp = 650.0f;
        m_hp = 650.0f;
        m_width = 72.0f;
        m_height = 136.0f;
    }
}

void Boss::TriggerDefeat() {
    m_state = BossState::DEFEATED;
    m_stateTimer = 3.5f;
    m_vel = { 0.0f, 0.0f };
}

void Boss::Update(float dt) {
    m_animTimer += dt;
    m_auraTimer += dt;

    if (m_hurtTimer > 0.0f) m_hurtTimer -= dt;
    if (m_flashTimer > 0.0f) m_flashTimer -= dt;
    if (m_invulnTimer > 0.0f) m_invulnTimer -= dt;
    if (m_attackCooldown > 0.0f) m_attackCooldown -= dt;

    // Starscream Aerial Flight Physics
    if (m_bossType == LevelBossType::STARSCREAM) {
        m_pos.x += m_vel.x * dt;
        m_pos.y += m_vel.y * dt;
        m_isGrounded = false;

        // Hover bobbing
        if (m_state == BossState::NEUTRAL || m_state == BossState::INTRO) {
            float targetY = m_flightHoverY + sinf(m_animTimer * 2.5f) * 45.0f;
            m_vel.y = (targetY - m_pos.y) * 2.0f;
        }

        // Flight boundary clamps
        if (m_pos.y < 120.0f) m_pos.y = 120.0f;
        if (m_pos.y > Constants::GROUND_Y - 30.0f) m_pos.y = Constants::GROUND_Y - 30.0f;

        if (m_pos.x - m_width * 0.5f < Constants::ARENA_MIN_X + 100.0f) {
            m_pos.x = Constants::ARENA_MIN_X + 100.0f + m_width * 0.5f;
            m_vel.x = 0.0f;
        } else if (m_pos.x + m_width * 0.5f > Constants::ARENA_MAX_X - 100.0f) {
            m_pos.x = Constants::ARENA_MAX_X - 100.0f - m_width * 0.5f;
            m_vel.x = 0.0f;
        }
        return;
    }

    // Ground Boss Physics (Shockwave, Soundwave, Megatron)
    if (m_pos.y < Constants::GROUND_Y) {
        m_isGrounded = false;
    } else {
        m_pos.y = Constants::GROUND_Y;
        if (m_vel.y > 0.0f) m_vel.y = 0.0f;
        m_isGrounded = true;
    }

    if (m_state != BossState::SPECIAL_ACTION) {
        if (!m_isGrounded || m_vel.y < 0.0f) {
            m_vel.y += Constants::GRAVITY * dt;
            m_isGrounded = false;
        }
    }

    if (m_vel.y < -850.0f) m_vel.y = -850.0f;
    if (m_vel.y > 1200.0f) m_vel.y = 1200.0f;

    m_pos.x += m_vel.x * dt;
    m_pos.y += m_vel.y * dt;

    if (m_pos.y >= Constants::GROUND_Y) {
        m_pos.y = Constants::GROUND_Y;
        if (m_vel.y > 0.0f) m_vel.y = 0.0f;
        m_isGrounded = true;
    }

    if (m_state != BossState::SPECIAL_ACTION) {
        if (m_vel.x > 0.0f) {
            m_vel.x = std::max(0.0f, m_vel.x - 1200.0f * dt);
        } else if (m_vel.x < 0.0f) {
            m_vel.x = std::min(0.0f, m_vel.x + 1200.0f * dt);
        }
    }

    if (m_pos.x - m_width * 0.5f < Constants::ARENA_MIN_X) {
        m_pos.x = Constants::ARENA_MIN_X + m_width * 0.5f;
        m_vel.x = 0.0f;
    } else if (m_pos.x + m_width * 0.5f > Constants::ARENA_MAX_X) {
        m_pos.x = Constants::ARENA_MAX_X - m_width * 0.5f;
        m_vel.x = 0.0f;
    }
}

void Boss::UpdateAI(Vector2 playerPos, float dt, SoundSystem& sounds, ParticleSystem& particles, std::vector<Projectile>& outProjectiles) {
    if (m_state == BossState::DEFEATED) {
        if (RandF() < 0.35f) {
            Vector2 expPos = { m_pos.x + (RandF() * 60.0f - 30.0f), m_pos.y - (RandF() * 110.0f) };
            particles.EmitSparks(expPos, 12, Constants::ENERGON_PURPLE, 100.0f, 320.0f);
            particles.EmitSmoke(expPos, { 0.0f, -40.0f }, 2, DARKGRAY, 10.0f);
            sounds.Play(SoundType::EXPLOSION, 1.2f, 0.4f);
        }
        return;
    }

    bool phaseTwo = IsInPhaseTwo();
    if (phaseTwo && !m_phaseTwoAnnounced) {
        m_phaseTwoAnnounced = true;
        sounds.Play(SoundType::BOSS_ROAR, 1.0f, 1.0f);
        particles.EmitShockwave({ m_pos.x, m_pos.y - 64.0f }, 140.0f, Constants::ENERGON_PURPLE, 0.45f, 6.0f);
    }

    if (phaseTwo && RandF() < 0.35f) {
        particles.EmitSparks({ m_pos.x + (RandF() * 50.0f - 25.0f), m_pos.y - (RandF() * 100.0f) }, 4, Constants::ENERGON_PURPLE, 50.0f, 180.0f);
    }

    float distToPlayer = fabsf(playerPos.x - m_pos.x);
    float dirToPlayer = (playerPos.x > m_pos.x) ? 1.0f : -1.0f;

    switch (m_state) {
        case BossState::INTRO: {
            m_stateTimer -= dt;
            if (m_stateTimer <= 0.0f) {
                m_state = BossState::NEUTRAL;
                sounds.Play(SoundType::BOSS_ROAR, 1.0f, 0.9f);
            }
            break;
        }

        case BossState::NEUTRAL: {
            m_facing = dirToPlayer;
            m_stateTimer -= dt;

            // Approach player
            float moveSpeed = phaseTwo ? 170.0f : 120.0f;
            if (m_bossType == LevelBossType::STARSCREAM) {
                // Strafe horizontally in air
                m_vel.x = m_facing * (moveSpeed * 1.5f);
            } else {
                m_vel.x = m_facing * moveSpeed;
            }

            if (m_attackCooldown <= 0.0f) {
                int choice = rand() % 3;

                if (m_bossType == LevelBossType::SHOCKWAVE) {
                    // Shockwave: 0 = Hyper-Beam, 1 = Logic Stomp, 2 = Rapid Pulse
                    if (choice == 0 || distToPlayer > 300.0f) {
                        m_state = BossState::TELEGRAPH_ATTACK_1;
                        m_stateTimer = phaseTwo ? 0.35f : 0.55f;
                        sounds.Play(SoundType::BOSS_BEAM, 1.2f, 0.8f);
                    } else {
                        m_state = BossState::TELEGRAPH_ATTACK_2;
                        m_stateTimer = phaseTwo ? 0.30f : 0.45f;
                        sounds.Play(SoundType::SLASH_HEAVY, 0.8f, 0.8f);
                    }
                } else if (m_bossType == LevelBossType::SOUNDWAVE) {
                    // Soundwave: 0 = Sonic Discs, 1 = Shoulder Rocket, 2 = Sonic Pulse
                    if (choice == 0) {
                        m_state = BossState::TELEGRAPH_ATTACK_1;
                        // Give the player a clear reaction window before the sonic disc.
                        m_stateTimer = phaseTwo ? 0.50f : 0.70f;
                        sounds.Play(SoundType::SONIC_BLAST, 1.0f, 0.9f);
                    } else {
                        m_state = BossState::TELEGRAPH_ATTACK_2;
                        // The two-disc salvo is more dangerous, so telegraph it longer.
                        m_stateTimer = phaseTwo ? 0.50f : 0.70f;
                        sounds.Play(SoundType::MISSILE_FIRE, 1.0f, 0.9f);
                    }
                } else if (m_bossType == LevelBossType::STARSCREAM) {
                    // Starscream: 0 = Null-Ray barrage, 1 = Seeker Missiles, 2 = Supersonic Dive Bomb
                    if (choice == 2 && distToPlayer < 400.0f) {
                        m_state = BossState::SPECIAL_ACTION; // Dive bomb
                        m_stateTimer = 0.8f;
                        sounds.Play(SoundType::BOSS_ROAR, 1.4f, 0.9f);
                        m_attackId++;
                    } else if (choice == 0) {
                        m_state = BossState::TELEGRAPH_ATTACK_1;
                        m_stateTimer = 0.4f;
                        sounds.Play(SoundType::BLASTER, 1.2f, 0.9f);
                    } else {
                        m_state = BossState::TELEGRAPH_ATTACK_2;
                        m_stateTimer = 0.45f;
                        sounds.Play(SoundType::MISSILE_FIRE, 1.1f, 0.9f);
                    }
                } else {
                    // Megatron: Fusion Cannon, Energon Mace, Jet Dash
                    if (distToPlayer > 350.0f || choice == 0) {
                        m_state = BossState::TELEGRAPH_ATTACK_1;
                        m_stateTimer = phaseTwo ? 0.35f : 0.55f;
                        sounds.Play(SoundType::BOSS_BEAM, 0.85f, 0.8f);
                    } else if (distToPlayer < 180.0f || choice == 1) {
                        m_state = BossState::TELEGRAPH_ATTACK_2;
                        m_stateTimer = phaseTwo ? 0.30f : 0.45f;
                        sounds.Play(SoundType::SLASH_HEAVY, 0.7f, 0.8f);
                    } else {
                        m_state = BossState::SPECIAL_ACTION; // Jet Dash
                        m_stateTimer = phaseTwo ? 0.65f : 0.85f;
                        sounds.Play(SoundType::BOSS_ROAR, 1.25f, 0.8f);
                        m_attackId++;
                    }
                }
            }
            break;
        }

        case BossState::TELEGRAPH_ATTACK_1: {
            m_vel.x = 0.0f;
            m_facing = dirToPlayer;
            m_stateTimer -= dt;

            Vector2 muzzle = { m_pos.x + m_facing * 50.0f, m_pos.y - 75.0f };
            particles.EmitSparks(muzzle, 3, Constants::ENERGON_PURPLE, 40.0f, 130.0f);

            if (m_stateTimer <= 0.0f) {
                m_state = BossState::EXECUTE_ATTACK_1;
                m_stateTimer = 0.30f;
                static int bossProjGen = 9000;

                if (m_bossType == LevelBossType::SHOCKWAVE) {
                    sounds.Play(SoundType::EXPLOSION, 1.4f, 0.9f);
                    sounds.Play(SoundType::BOSS_BEAM, 1.3f, 1.0f);
                    Vector2 beamVel = { m_facing * 980.0f, 0.0f };
                    outProjectiles.emplace_back(bossProjGen++, Faction::ENEMY, muzzle, beamVel, ProjectileType::SHOCKWAVE_HYPER_BEAM, 35.0f, 2.5f);
                    particles.EmitShockwave(muzzle, 75.0f, { 180, 40, 240, 255 }, 0.25f, 5.0f);
                } else if (m_bossType == LevelBossType::SOUNDWAVE) {
                    sounds.Play(SoundType::SONIC_BLAST, 1.0f, 1.0f);
                    Vector2 discVel = { m_facing * 650.0f, 0.0f };
                    outProjectiles.emplace_back(bossProjGen++, Faction::ENEMY, muzzle, discVel, ProjectileType::SOUNDWAVE_SONIC_DISC, 18.0f, 3.0f);
                    particles.EmitShockwave(muzzle, 60.0f, Constants::ENERGON_CYAN, 0.3f, 4.0f);
                } else if (m_bossType == LevelBossType::STARSCREAM) {
                    sounds.Play(SoundType::BLASTER, 1.3f, 1.0f);
                    for (int s = -1; s <= 1; s += 2) {
                        Vector2 rayVel = { m_facing * 880.0f, (float)s * 90.0f };
                        outProjectiles.emplace_back(bossProjGen++, Faction::ENEMY, muzzle, rayVel, ProjectileType::STARSCREAM_NULL_RAY, 22.0f, 2.5f);
                    }
                    particles.EmitShockwave(muzzle, 65.0f, { 255, 40, 40, 255 }, 0.25f, 4.0f);
                } else { // MEGATRON
                    sounds.Play(SoundType::EXPLOSION, 1.1f, 1.0f);
                    sounds.Play(SoundType::BOSS_BEAM, 1.1f, 1.0f);
                    Vector2 orbVel = { m_facing * 780.0f, 0.0f };
                    outProjectiles.emplace_back(bossProjGen++, Faction::ENEMY, muzzle, orbVel, ProjectileType::BOSS_FUSION_ORB, 34.0f, 3.0f);
                    particles.EmitShockwave(muzzle, 70.0f, Constants::ENERGON_PURPLE, 0.25f, 5.0f);
                }
                m_vel.x = -m_facing * 90.0f; // Recoil kick
            }
            break;
        }

        case BossState::EXECUTE_ATTACK_1: {
            m_stateTimer -= dt;
            if (m_stateTimer <= 0.0f) {
                m_state = BossState::NEUTRAL;
                m_attackCooldown = (m_bossType == LevelBossType::SOUNDWAVE)
                    ? (phaseTwo ? 1.35f : 1.90f)
                    : (phaseTwo ? 0.9f : 1.6f);
            }
            break;
        }

        case BossState::TELEGRAPH_ATTACK_2: {
            m_vel.x = 0.0f;
            m_facing = dirToPlayer;
            m_stateTimer -= dt;

            if (m_stateTimer <= 0.0f) {
                m_state = BossState::EXECUTE_ATTACK_2;
                m_stateTimer = 0.35f;
                m_attackId++;
                static int bossProjGen2 = 9500;

                if (m_bossType == LevelBossType::SHOCKWAVE) {
                    // Logic Ground Stomp
                    sounds.Play(SoundType::TRUCK_RAM, 0.8f, 1.0f);
                    Vector2 stompPos = { m_pos.x + m_facing * 40.0f, Constants::GROUND_Y - 4.0f };
                    particles.EmitShockwave(stompPos, 90.0f, { 180, 40, 240, 255 }, 0.35f, 5.0f);
                    particles.EmitSparks(stompPos, 20, { 255, 225, 40, 255 }, 120.0f, 350.0f);
                } else if (m_bossType == LevelBossType::SOUNDWAVE) {
                    // Shoulder Rocket Salvo
                    sounds.Play(SoundType::MISSILE_FIRE, 1.0f, 1.0f);
                    Vector2 shMuzzle = { m_pos.x + m_facing * 20.0f, m_pos.y - 120.0f };
                    outProjectiles.emplace_back(bossProjGen2++, Faction::ENEMY, shMuzzle, Vector2{ m_facing * 620.0f, -60.0f }, ProjectileType::SOUNDWAVE_SONIC_DISC, 16.0f, 3.0f);
                    outProjectiles.emplace_back(bossProjGen2++, Faction::ENEMY, shMuzzle, Vector2{ m_facing * 620.0f, 60.0f }, ProjectileType::SOUNDWAVE_SONIC_DISC, 16.0f, 3.0f);
                } else if (m_bossType == LevelBossType::STARSCREAM) {
                    // Seeker Missile Salvo
                    sounds.Play(SoundType::MISSILE_FIRE, 1.1f, 1.0f);
                    Vector2 wingMuzzle = { m_pos.x + m_facing * 35.0f, m_pos.y - 65.0f };
                    outProjectiles.emplace_back(bossProjGen2++, Faction::ENEMY, wingMuzzle, Vector2{ m_facing * 640.0f, -40.0f }, ProjectileType::STARSCREAM_MISSILE, 30.0f, 3.0f);
                    outProjectiles.emplace_back(bossProjGen2++, Faction::ENEMY, wingMuzzle, Vector2{ m_facing * 640.0f, 40.0f }, ProjectileType::STARSCREAM_MISSILE, 30.0f, 3.0f);
                } else { // MEGATRON Energon Mace
                    sounds.Play(SoundType::SLASH_HEAVY, 0.8f, 1.0f);
                    Vector2 slamPos = { m_pos.x + m_facing * 55.0f, Constants::GROUND_Y - 4.0f };
                    particles.EmitShockwave(slamPos, 80.0f, Constants::ENERGON_PURPLE, 0.28f, 5.0f);
                    particles.EmitSparks(slamPos, 18, Constants::ENERGON_PURPLE, 140.0f, 360.0f);
                }
            }
            break;
        }

        case BossState::EXECUTE_ATTACK_2: {
            m_stateTimer -= dt;
            if (m_stateTimer <= 0.0f) {
                m_state = BossState::NEUTRAL;
                m_attackCooldown = (m_bossType == LevelBossType::SOUNDWAVE)
                    ? (phaseTwo ? 1.35f : 1.90f)
                    : (phaseTwo ? 0.9f : 1.5f);
            }
            break;
        }

        case BossState::SPECIAL_ACTION: {
            m_stateTimer -= dt;

            if (m_bossType == LevelBossType::STARSCREAM) {
                // Supersonic Dive Bomb
                float diveSpeed = phaseTwo ? 850.0f : 700.0f;
                m_vel.x = m_facing * diveSpeed;
                m_vel.y = 250.0f; // Dive downwards towards player
                particles.EmitSparks({ m_pos.x, m_pos.y - 40.0f }, 5, { 255, 40, 40, 255 }, 120.0f, 280.0f);

                if (m_stateTimer <= 0.0f || m_pos.y >= Constants::GROUND_Y - 40.0f) {
                    m_state = BossState::NEUTRAL;
                    m_attackCooldown = 1.4f;
                    m_vel = { 0.0f, -200.0f }; // Ascend back
                }
            } else {
                // Megatron Jet Dash
                float dashSpeed = phaseTwo ? 740.0f : 580.0f;
                m_vel.x = m_facing * dashSpeed;
                Vector2 exhaust = { m_pos.x - m_facing * 28.0f, m_pos.y - 80.0f };
                particles.EmitSparks(exhaust, 4, Constants::DECEPTICON_PURPLE, 120.0f, 260.0f);

                if (m_stateTimer <= 0.0f) {
                    m_state = BossState::NEUTRAL;
                    m_attackCooldown = phaseTwo ? 1.0f : 1.8f;
                    m_vel.x = 0.0f;
                }
            }
            break;
        }

        default:
            break;
    }
}

Hitbox Boss::GetMeleeHitbox() const {
    Hitbox hb;
    hb.active = false;
    // Soundwave's second attack is a ranged two-disc salvo, not a melee strike.
    // Do not generate an invisible close-range hitbox during that projectile attack.
    if (m_bossType == LevelBossType::SOUNDWAVE || m_state != BossState::EXECUTE_ATTACK_2) return hb;

    float reachX = 90.0f;
    float reachH = 85.0f;
    float hitX = (m_facing > 0.0f) ? m_pos.x : (m_pos.x - reachX);
    float hitY = m_pos.y - reachH;

    hb.rect = { hitX, hitY, reachX, reachH };
    hb.faction = Faction::ENEMY;
    hb.damage = 30.0f;
    hb.knockback = { m_facing * 420.0f, -220.0f };
    hb.isCrit = true;
    hb.active = true;
    hb.attackId = m_attackId;
    hb.hitStopDuration = Constants::HIT_STOP_HEAVY;
    hb.causesScreenShake = true;
    return hb;
}

Hitbox Boss::GetSpecialHitbox() const {
    Hitbox hb;
    hb.active = false;
    if (m_state != BossState::SPECIAL_ACTION) return hb;

    hb.rect = GetBounds();
    hb.faction = Faction::ENEMY;
    hb.damage = 28.0f;
    hb.knockback = { m_facing * 460.0f, -180.0f };
    hb.isCrit = true;
    hb.active = true;
    hb.attackId = m_attackId;
    hb.hitStopDuration = Constants::HIT_STOP_HEAVY;
    hb.causesScreenShake = true;
    return hb;
}

// -------------------------------------------------------------
// 1. SHOCKWAVE PROCEDURAL RENDERING (Reference: media_..._de305228.jpg)
// -------------------------------------------------------------
void Boss::DrawShockwave() const {
    float f = m_facing;
    Vector2 p = m_pos;

    if (m_flashTimer > 0.0f && ((int)(m_flashTimer * 30.0f) % 2 == 0)) {
        DrawRectangleRounded({ p.x - 34.0f, p.y - 132.0f, 68.0f, 132.0f }, 0.2f, 4, RAYWHITE);
        return;
    }

    Color purple = { 95, 30, 150, 255 };
    Color darkPurple = { 55, 18, 90, 255 };
    Color magenta = { 225, 40, 240, 255 };
    Color silver = { 185, 195, 210, 255 };
    Color black = { 25, 28, 35, 255 };
    Color eyeGold = { 255, 225, 40, 255 };

    // 1. Heavy Angular Legs & Sabatons
    // Thighs (silver mechanics)
    DrawRectangle((int)(p.x - 22.0f), (int)(p.y - 68.0f), 18, 26, silver);
    DrawRectangle((int)(p.x + 4.0f), (int)(p.y - 68.0f), 18, 26, silver);

    // Purple Shin Armor Plates
    DrawRectangle((int)(p.x - 26.0f), (int)(p.y - 44.0f), 22, 34, purple);
    DrawRectangle((int)(p.x + 4.0f), (int)(p.y - 44.0f), 22, 34, purple);
    DrawRectangleLines((int)(p.x - 26.0f), (int)(p.y - 44.0f), 22, 34, darkPurple);
    DrawRectangleLines((int)(p.x + 4.0f), (int)(p.y - 44.0f), 22, 34, darkPurple);

    // Boots / Sabatons (properly oriented with facing)
    float foot1X = (f > 0.0f) ? (p.x - 28.0f) : (p.x - 34.0f);
    float foot2X = (f > 0.0f) ? (p.x + 2.0f) : (p.x - 4.0f);
    DrawRectangle((int)foot1X, (int)(p.y - 12.0f), 26, 12, black);
    DrawRectangle((int)foot2X, (int)(p.y - 12.0f), 26, 12, black);

    // 2. Abdomen & Hip Armor
    DrawRectangle((int)(p.x - 18.0f), (int)(p.y - 78.0f), 36, 12, darkPurple);

    // 3. Torso Carapace with Glowing Ribbed Core
    DrawRectangleRounded({ p.x - 30.0f, p.y - 114.0f, 60.0f, 38.0f }, 0.2f, 4, darkPurple);
    DrawRectangleRoundedLinesEx({ p.x - 30.0f, p.y - 114.0f, 60.0f, 38.0f }, 0.2f, 4, 1.5f, purple);

    // Glowing Horizontal Magenta Core with segmented ribs
    float pulse = (sinf(m_animTimer * 5.0f) + 1.0f) * 0.5f;
    Color glowMag = ColorAlpha(magenta, 0.75f + 0.25f * pulse);
    DrawRectangle((int)(p.x - 22.0f), (int)(p.y - 104.0f), 44, 16, glowMag);
    for (int r = 0; r < 3; ++r) {
        DrawLine((int)(p.x - 22.0f), (int)(p.y - 100.0f + r * 5.0f), (int)(p.x + 22.0f), (int)(p.y - 100.0f + r * 5.0f), black);
    }

    // 4. Hexagonal Head & Single Glowing Cyclops Optic
    DrawRectangle((int)(p.x - 12.0f), (int)(p.y - 132.0f), 24, 18, darkPurple);
    // Antenna fins on head
    DrawRectangle((int)(p.x - 15.0f), (int)(p.y - 138.0f), 3, 16, silver);
    DrawRectangle((int)(p.x + 12.0f), (int)(p.y - 138.0f), 3, 16, silver);
    // Single Cyclops Eye
    DrawCircle((int)(p.x + f * 3.0f), (int)(p.y - 124.0f), 5.5f, eyeGold);
    DrawCircle((int)(p.x + f * 3.0f), (int)(p.y - 124.0f), 2.5f, RAYWHITE);

    // 5. Broad Angular Shoulders
    DrawRectangleRounded({ p.x - f * 38.0f - 8.0f, p.y - 118.0f, 22.0f, 22.0f }, 0.2f, 4, purple);
    DrawRectangleRounded({ p.x + f * 20.0f - 8.0f, p.y - 118.0f, 24.0f, 24.0f }, 0.2f, 4, purple);

    // 6. Arms: Left arm is massive Hyper-Cannon; Right arm is gauntlet
    Vector2 cannonArm = { p.x + f * 24.0f, p.y - 96.0f };
    DrawShockwaveArmCannon(cannonArm);
}

void Boss::DrawShockwaveArmCannon(Vector2 armPos) const {
    float f = m_facing;
    Color darkPurple = { 55, 18, 90, 255 };
    Color purple = { 105, 35, 165, 255 };
    Color magenta = { 230, 40, 240, 255 };
    Color silver = { 190, 200, 215, 255 };

    float len = 52.0f;
    float h = 18.0f;
    float minX = (f > 0.0f) ? armPos.x : (armPos.x - len);
    float minY = armPos.y - 9.0f;

    // Cannon Barrel
    DrawRectangle((int)minX, (int)minY, (int)len, (int)h, purple);
    DrawRectangleLines((int)minX, (int)minY, (int)len, (int)h, darkPurple);

    // Glowing core stripe
    DrawRectangle((int)minX + 8, (int)minY + 4, (int)len - 16, 6, magenta);

    // Pointed hyper-flux nozzle tip
    float tipX = (f > 0.0f) ? (minX + len) : minX;
    Vector2 p1 = { tipX, minY - 2.0f };
    Vector2 p2 = { tipX + f * 12.0f, minY + h * 0.5f };
    Vector2 p3 = { tipX, minY + h + 2.0f };
    DrawTriangle(p1, (f > 0.0f ? p3 : p2), (f > 0.0f ? p2 : p3), silver);

    // Heavy power hose to back
    DrawLineEx({ armPos.x - f * 8.0f, armPos.y + 4.0f }, { armPos.x - f * 24.0f, armPos.y - 14.0f }, 3.5f, darkPurple);
}

// -------------------------------------------------------------
// 2. SOUNDWAVE PROCEDURAL RENDERING (Reference: media_..._9ac122f9.jpg)
// -------------------------------------------------------------
void Boss::DrawSoundwave() const {
    float f = m_facing;
    Vector2 p = m_pos;

    if (m_flashTimer > 0.0f && ((int)(m_flashTimer * 30.0f) % 2 == 0)) {
        DrawRectangleRounded({ p.x - 32.0f, p.y - 128.0f, 64.0f, 128.0f }, 0.2f, 4, RAYWHITE);
        return;
    }

    Color blue = { 25, 75, 185, 255 };
    Color darkBlue = { 14, 42, 110, 255 };
    Color silver = { 200, 210, 225, 255 };
    Color gold = { 240, 185, 25, 255 };
    Color glass = { 30, 40, 55, 240 };
    Color red = { 230, 30, 45, 255 };

    // 1. Legs: Silver thighs & Heavy Blue Shins with knee vents
    DrawRectangle((int)(p.x - 22.0f), (int)(p.y - 66.0f), 18, 24, silver);
    DrawRectangle((int)(p.x + 4.0f), (int)(p.y - 66.0f), 18, 24, silver);

    // Blue shins
    DrawRectangle((int)(p.x - 24.0f), (int)(p.y - 44.0f), 20, 34, blue);
    DrawRectangle((int)(p.x + 4.0f), (int)(p.y - 44.0f), 20, 34, blue);
    // Red knee markers
    DrawRectangle((int)(p.x - 20.0f), (int)(p.y - 42.0f), 12, 6, red);
    DrawRectangle((int)(p.x + 8.0f), (int)(p.y - 42.0f), 12, 6, red);

    // Blue Sabatons
    float foot1X = (f > 0.0f) ? (p.x - 26.0f) : (p.x - 32.0f);
    float foot2X = (f > 0.0f) ? (p.x + 2.0f) : (p.x - 4.0f);
    DrawRectangle((int)foot1X, (int)(p.y - 12.0f), 24, 12, blue);
    DrawRectangle((int)foot2X, (int)(p.y - 12.0f), 24, 12, blue);

    // 2. Torso with Iconic Gold Cassette Door & Animated Equalizer
    DrawRectangleRounded({ p.x - 28.0f, p.y - 110.0f, 56.0f, 44.0f }, 0.15f, 4, blue);
    DrawRectangleRoundedLinesEx({ p.x - 28.0f, p.y - 110.0f, 56.0f, 44.0f }, 0.15f, 4, 1.5f, darkBlue);

    // Golden Cassette Door Frame
    Rectangle cassetteFrame = { p.x - 20.0f, p.y - 104.0f, 40.0f, 28.0f };
    DrawRectangleRec(cassetteFrame, glass);
    DrawRectangleLinesEx(cassetteFrame, 2.5f, gold);

    // Animated Equalizer Frequency Waveform Bars
    for (int b = 0; b < 6; ++b) {
        float barH = 6.0f + fabsf(sinf(m_animTimer * 6.0f + (float)b * 0.9f)) * 14.0f;
        float bx = cassetteFrame.x + 4.0f + b * 6.0f;
        float by = cassetteFrame.y + cassetteFrame.height - barH - 4.0f;
        DrawRectangle((int)bx, (int)by, 4, (int)barH, Constants::ENERGON_CYAN);
    }

    // 3. Blue Helmet with Red Visor
    DrawRectangle((int)(p.x - 11.0f), (int)(p.y - 128.0f), 22, 18, blue);
    DrawRectangle((int)(p.x - 8.0f + f * 2.0f), (int)(p.y - 122.0f), 14, 4, red); // Visor
    DrawRectangle((int)(p.x - 6.0f + f * 2.0f), (int)(p.y - 117.0f), 10, 6, silver); // Faceplate

    // 4. Shoulders with Speaker Discs
    DrawRectangleRounded({ p.x - f * 36.0f - 6.0f, p.y - 114.0f, 20.0f, 20.0f }, 0.3f, 4, darkBlue);
    DrawRectangleRounded({ p.x + f * 18.0f - 6.0f, p.y - 114.0f, 22.0f, 22.0f }, 0.3f, 4, blue);
    // Speaker discs
    DrawCircle((int)(p.x + f * 24.0f), (int)(p.y - 104.0f), 6.0f, red);
    DrawCircle((int)(p.x + f * 24.0f), (int)(p.y - 104.0f), 3.0f, silver);

    // 5. Right Shoulder-Mounted Cylindrical Sonic Cannon
    Vector2 shPos = { p.x - f * 12.0f, p.y - 116.0f };
    DrawSoundwaveShoulderCannon(shPos);
}

void Boss::DrawSoundwaveShoulderCannon(Vector2 shoulderPos) const {
    float f = m_facing;
    Color silver = { 210, 220, 235, 255 };
    Color darkBlue = { 14, 42, 110, 255 };
    Color red = { 230, 30, 45, 255 };

    float len = 38.0f;
    float h = 14.0f;
    float minX = (f > 0.0f) ? (shoulderPos.x - 10.0f) : (shoulderPos.x - len + 10.0f);
    float minY = shoulderPos.y - 14.0f;

    DrawRectangle((int)minX, (int)minY, (int)len, (int)h, silver);
    DrawRectangleLines((int)minX, (int)minY, (int)len, (int)h, darkBlue);

    // Muzzle ring & targeting sensor
    float muzX = (f > 0.0f) ? (minX + len - 4.0f) : minX;
    DrawRectangle((int)muzX, (int)minY - 1, 4, (int)h + 2, red);
}

// -------------------------------------------------------------
// 3. STARSCREAM PROCEDURAL RENDERING (Reference: media_..._eb0de9fb.jpg)
// -------------------------------------------------------------
void Boss::DrawStarscream() const {
    float f = m_facing;
    Vector2 p = m_pos;

    if (m_flashTimer > 0.0f && ((int)(m_flashTimer * 30.0f) % 2 == 0)) {
        DrawRectangleRounded({ p.x - 36.0f, p.y - 126.0f, 72.0f, 126.0f }, 0.2f, 4, RAYWHITE);
        return;
    }

    Color white = { 235, 240, 245, 255 };
    Color red = Constants::AUTOBOT_RED;
    Color blue = Constants::AUTOBOT_BLUE;
    Color cockpitGold = { 245, 195, 30, 230 };
    Color darkGrey = { 45, 48, 55, 255 };

    // 1. Back Swept Delta Jet Wings (Massive F-22 wings on back)
    DrawStarscreamWings(p);

    // 2. Aerodynamic Jet Legs & Thruster Boots
    DrawRectangle((int)(p.x - 20.0f), (int)(p.y - 62.0f), 16, 26, white);
    DrawRectangle((int)(p.x + 4.0f), (int)(p.y - 62.0f), 16, 26, white);

    // Blue Shin Cowlings
    DrawRectangle((int)(p.x - 22.0f), (int)(p.y - 42.0f), 18, 32, blue);
    DrawRectangle((int)(p.x + 4.0f), (int)(p.y - 42.0f), 18, 32, blue);

    // Aerodynamic Sabatons
    float foot1X = (f > 0.0f) ? (p.x - 24.0f) : (p.x - 30.0f);
    float foot2X = (f > 0.0f) ? (p.x + 2.0f) : (p.x - 4.0f);
    DrawRectangle((int)foot1X, (int)(p.y - 10.0f), 24, 10, darkGrey);
    DrawRectangle((int)foot2X, (int)(p.y - 10.0f), 24, 10, darkGrey);

    // 3. Torso: Red Carapace with Golden Jet Cockpit Canopy in Center
    DrawRectangleRounded({ p.x - 26.0f, p.y - 108.0f, 52.0f, 44.0f }, 0.2f, 4, red);
    DrawRectangleRoundedLinesEx({ p.x - 26.0f, p.y - 108.0f, 52.0f, 44.0f }, 0.2f, 4, 1.5f, { 160, 20, 25, 255 });

    // Golden Cockpit Glass Canopy
    DrawRectangleRounded({ p.x - 10.0f, p.y - 100.0f, 20.0f, 24.0f }, 0.4f, 4, cockpitGold);
    DrawRectangleRoundedLinesEx({ p.x - 10.0f, p.y - 100.0f, 20.0f, 24.0f }, 0.4f, 4, 1.5f, white);

    // 4. Dark Head with Glowing Crimson Optics
    DrawRectangle((int)(p.x - 10.0f), (int)(p.y - 126.0f), 20, 18, darkGrey);
    DrawRectangle((int)(p.x - 7.0f + f * 3.0f), (int)(p.y - 120.0f), 10, 4, { 255, 10, 20, 255 }); // Crimson eyes

    // 5. Arms with Arm-Mounted Null-Ray Cannons
    Vector2 rightArmPos = { p.x + f * 22.0f, p.y - 92.0f };
    float nrW = 44.0f;
    float nrMinX = (f > 0.0f) ? rightArmPos.x : (rightArmPos.x - nrW);
    DrawRectangle((int)nrMinX, (int)rightArmPos.y - 5, (int)nrW, 10, darkGrey);
    DrawRectangle((int)nrMinX + 4, (int)rightArmPos.y - 3, (int)nrW - 8, 4, blue);
    float muzzleX = (f > 0.0f) ? (nrMinX + nrW - 4.0f) : nrMinX;
    DrawRectangle((int)muzzleX, (int)rightArmPos.y - 6, 4, 12, red);
}

void Boss::DrawStarscreamWings(Vector2 center) const {
    float f = m_facing;
    Color white = { 235, 240, 245, 255 };
    Color red = Constants::AUTOBOT_RED;
    Color darkGrey = { 45, 48, 55, 255 };

    // Left Wing
    Vector2 wRoot = { center.x - f * 12.0f, center.y - 95.0f };
    Vector2 wTip1 = { center.x - f * 65.0f, center.y - 145.0f };
    Vector2 wTip2 = { center.x - f * 50.0f, center.y - 70.0f };

    DrawTriangle(wRoot, (f > 0.0f ? wTip1 : wTip2), (f > 0.0f ? wTip2 : wTip1), white);
    DrawLineEx(wRoot, wTip1, 3.5f, red);
    DrawLineEx(wTip1, wTip2, 2.0f, darkGrey);

    // Right Wing
    Vector2 wRootR = { center.x + f * 12.0f, center.y - 95.0f };
    Vector2 wTip1R = { center.x + f * 48.0f, center.y - 135.0f };
    Vector2 wTip2R = { center.x + f * 38.0f, center.y - 75.0f };

    DrawTriangle(wRootR, (f > 0.0f ? wTip2R : wTip1R), (f > 0.0f ? wTip1R : wTip2R), white);
    DrawLineEx(wRootR, wTip1R, 3.0f, red);
}

// -------------------------------------------------------------
// 4. MEGATRON PROCEDURAL RENDERING (Reference: media_..._f99be549.png)
// -------------------------------------------------------------
void Boss::DrawMegatron() const {
    float f = m_facing;
    Vector2 p = m_pos;

    if (m_flashTimer > 0.0f && ((int)(m_flashTimer * 30.0f) % 2 == 0)) {
        DrawRectangleRounded({ p.x - 36.0f, p.y - 136.0f, 72.0f, 136.0f }, 0.2f, 4, RAYWHITE);
        return;
    }

    Color gunmetal = { 130, 140, 155, 255 };
    Color darkSteel = { 45, 50, 60, 255 };
    Color chrome = { 200, 210, 225, 255 };
    Color red = { 230, 25, 35, 255 };

    // Overdrive Electric Aura in Phase 2
    if (IsInPhaseTwo()) {
        float pulse = (sinf(m_auraTimer * 10.0f) + 1.0f) * 0.5f;
        Color auraCol = ColorAlpha(Constants::DECEPTICON_PURPLE, 0.25f + 0.25f * pulse);
        DrawCircle((int)p.x, (int)(p.y - 68.0f), 78.0f + pulse * 8.0f, auraCol);
    }

    // 1. Armored Legs & Heavy Sabatons
    DrawRectangle((int)(p.x - 26.0f), (int)(p.y - 68.0f), 22, 68, darkSteel);
    DrawRectangle((int)(p.x + 4.0f), (int)(p.y - 68.0f), 22, 68, darkSteel);
    DrawRectangle((int)(p.x - 24.0f), (int)(p.y - 46.0f), 18, 14, gunmetal);
    DrawRectangle((int)(p.x + 6.0f), (int)(p.y - 46.0f), 18, 14, gunmetal);

    float foot1X = (f > 0.0f) ? (p.x - 30.0f) : (p.x - 36.0f);
    float foot2X = (f > 0.0f) ? (p.x + 2.0f) : (p.x - 4.0f);
    DrawRectangle((int)foot1X, (int)(p.y - 14.0f), 28, 14, chrome);
    DrawRectangle((int)foot2X, (int)(p.y - 14.0f), 28, 14, chrome);

    // 2. Abdomen & Waist Plates
    DrawRectangle((int)(p.x - 20.0f), (int)(p.y - 80.0f), 40, 14, darkSteel);

    // 3. Torso with Twin Red Pectoral Intake Vents
    DrawRectangleRounded({ p.x - 30.0f, p.y - 116.0f, 60.0f, 38.0f }, 0.2f, 4, gunmetal);
    DrawRectangleRoundedLinesEx({ p.x - 30.0f, p.y - 116.0f, 60.0f, 38.0f }, 0.2f, 4, 1.5f, chrome);

    // Red Intake Vents on Pectorals
    DrawRectangle((int)(p.x - 24.0f), (int)(p.y - 108.0f), 14, 8, red);
    DrawRectangle((int)(p.x + 10.0f), (int)(p.y - 108.0f), 14, 8, red);

    // Decepticon Crest
    DrawTriangle({ p.x, p.y - 94.0f }, { p.x - 6.0f, p.y - 104.0f }, { p.x + 6.0f, p.y - 104.0f }, Constants::DECEPTICON_PURPLE);

    // 4. Iconic Megatron Crowned Bucket Helmet & Crimson Eyes
    DrawRectangle((int)(p.x - 13.0f), (int)(p.y - 136.0f), 26, 20, gunmetal);
    DrawTriangle({ p.x - 15.0f, p.y - 136.0f }, { p.x, p.y - 144.0f }, { p.x + 15.0f, p.y - 136.0f }, chrome);
    DrawRectangle((int)(p.x - 8.0f + f * 4.0f), (int)(p.y - 128.0f), 10, 4, { 255, 10, 20, 255 });

    // 5. Shoulder Pauldrons
    DrawRectangleRounded({ p.x - f * 38.0f - 8.0f, p.y - 120.0f, 22.0f, 22.0f }, 0.3f, 4, darkSteel);
    DrawRectangleRounded({ p.x + f * 20.0f - 8.0f, p.y - 120.0f, 26.0f, 26.0f }, 0.3f, 4, gunmetal);

    // 6. Fusion Cannon & Energon Mace
    Vector2 rightArmPos = { p.x + f * 26.0f, p.y - 98.0f };
    DrawMegatronFusionCannon(rightArmPos);

    if (m_state == BossState::TELEGRAPH_ATTACK_2 || m_state == BossState::EXECUTE_ATTACK_2) {
        DrawMegatronEnergonMace(rightArmPos);
    }
}

void Boss::DrawMegatronFusionCannon(Vector2 armPos) const {
    float f = m_facing;
    Color cannonBlack = { 28, 30, 38, 255 };
    Color glowPurple = Constants::ENERGON_PURPLE;

    float cannonLen = 54.0f;
    float cannonH = 22.0f;
    float minX = (f > 0.0f) ? armPos.x : (armPos.x - cannonLen);
    float minY = armPos.y - 11.0f;

    DrawRectangle((int)minX, (int)minY, (int)cannonLen, (int)cannonH, cannonBlack);
    DrawRectangleLines((int)minX, (int)minY, (int)cannonLen, (int)cannonH, { 80, 85, 100, 255 });

    float pulse = (sinf(m_animTimer * 6.0f) + 1.0f) * 0.5f;
    Color coreCol = ColorAlpha(glowPurple, 0.6f + 0.4f * pulse);
    float coreX = (f > 0.0f) ? (minX + 12.0f) : (minX + cannonLen - 26.0f);
    DrawRectangle((int)coreX, (int)minY + 5, 16, 12, coreCol);

    float muzX = (f > 0.0f) ? (minX + cannonLen - 5.0f) : minX;
    DrawRectangle((int)muzX, (int)minY - 2, 5, (int)cannonH + 4, DARKGRAY);
}

void Boss::DrawMegatronEnergonMace(Vector2 handPos) const {
    float f = m_facing;
    Color purple = Constants::ENERGON_PURPLE;

    Vector2 maceBall = { handPos.x + f * 55.0f, handPos.y + (m_state == BossState::EXECUTE_ATTACK_2 ? 25.0f : -35.0f) };
    DrawLineEx(handPos, maceBall, 3.5f, purple);
    DrawCircle((int)maceBall.x, (int)maceBall.y, 16.0f, purple);
    DrawCircle((int)maceBall.x, (int)maceBall.y, 8.0f, RAYWHITE);

    for (int s = 0; s < 6; s++) {
        float angle = (float)s * (3.14159f / 3.0f);
        Vector2 spikeTip = { maceBall.x + cosf(angle) * 22.0f, maceBall.y + sinf(angle) * 22.0f };
        DrawLineEx(maceBall, spikeTip, 3.0f, RAYWHITE);
    }
}

void Boss::Draw() const {
    switch (m_bossType) {
        case LevelBossType::SHOCKWAVE:
            DrawShockwave();
            break;
        case LevelBossType::SOUNDWAVE:
            DrawSoundwave();
            break;
        case LevelBossType::STARSCREAM:
            DrawStarscream();
            break;
        case LevelBossType::MEGATRON:
        default:
            DrawMegatron();
            break;
    }
}
