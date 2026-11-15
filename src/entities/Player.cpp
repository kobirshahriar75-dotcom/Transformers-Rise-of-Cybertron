#include "Player.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace {
    inline float RandFloat() {
        return (float)rand() / (float)RAND_MAX;
    }
}

Player::Player(int id, Vector2 pos)
    : Entity(id, Faction::PLAYER, pos, Constants::ROBOT_WIDTH, Constants::ROBOT_HEIGHT, Constants::PLAYER_MAX_HP)
    , m_mode(PlayerMode::ENERGON_BLADE)
    , m_energy(Constants::PLAYER_MAX_ENERGY)
    , m_jetpackMode(false)
    , m_animTimer(0.0f)
    , m_walkCycle(0.0f)
    , m_canDoubleJump(true)
    , m_isBoosting(false)
    , m_boostTimer(0.0f)
    , m_wheelRotation(0.0f)
    , m_isAttacking(false)
    , m_comboStep(0)
    , m_attackTimer(0.0f)
    , m_attackDuration(0.24f)
    , m_comboWindowTimer(0.0f)
    , m_currentAttackId(1)
    , m_blasterCooldown(0.0f)
    , m_transformAnimTimer(0.0f)
{
}

void Player::Reset(Vector2 pos) {
    m_pos = pos;
    m_vel = { 0.0f, 0.0f };
    m_hp = Constants::PLAYER_MAX_HP;
    m_energy = Constants::PLAYER_MAX_ENERGY;
    m_isDead = false;
    m_isGrounded = true;
    m_facing = 1.0f;
    m_mode = PlayerMode::ENERGON_BLADE;
    m_width = Constants::ROBOT_WIDTH;
    m_height = Constants::ROBOT_HEIGHT;
    m_isAttacking = false;
    m_comboStep = 0;
    m_attackTimer = 0.0f;
    m_comboWindowTimer = 0.0f;
    m_blasterCooldown = 0.0f;
    m_isBoosting = false;
    m_boostTimer = 0.0f;
    m_invulnTimer = 0.0f;
    m_flashTimer = 0.0f;
    m_hurtTimer = 0.0f;
    m_jetpackMode = false;
}

void Player::TransformTo(PlayerMode newMode, SoundSystem& sounds, ParticleSystem& particles) {
    // Aerial campaign levels have one fixed Jetpack configuration; standard
    // ground transformations are intentionally unavailable there.
    if (m_jetpackMode || m_mode == newMode) return;

    m_mode = newMode;
    m_transformAnimTimer = 0.35f;
    sounds.Play(SoundType::TRANSFORM, 1.0f, 1.0f);

    particles.EmitSparks(
        { m_pos.x, m_pos.y - m_height * 0.5f },
        18,
        Constants::ENERGON_CYAN,
        150.0f,
        350.0f
    );
    particles.EmitShockwave(
        { m_pos.x, m_pos.y - m_height * 0.4f },
        60.0f,
        Constants::AUTOBOT_RED,
        0.25f,
        3.0f
    );

    if (m_mode == PlayerMode::TRUCK) {
        m_width = Constants::TRUCK_WIDTH;
        m_height = Constants::TRUCK_HEIGHT;
    } else {
        m_width = Constants::ROBOT_WIDTH;
        m_height = Constants::ROBOT_HEIGHT;
    }
}

void Player::CycleMode(SoundSystem& sounds, ParticleSystem& particles) {
    if (m_jetpackMode) return;
    int nextMode = ((int)m_mode + 1) % 3;
    TransformTo((PlayerMode)nextMode, sounds, particles);
}

void Player::HandleInput(SoundSystem& sounds, ParticleSystem& particles, std::vector<Projectile>& outProjectiles) {
    if (m_isDead) return;

    // Ground campaign levels expose exactly three selectable modes.
    // Levels 7-8 lock Optimus into the single Jetpack aerial configuration.
    if (!m_jetpackMode) {
        if (IsKeyPressed(KEY_ONE)) {
            TransformTo(PlayerMode::ENERGON_BLADE, sounds, particles);
        } else if (IsKeyPressed(KEY_TWO)) {
            TransformTo(PlayerMode::PLASMA_CANNON, sounds, particles);
        } else if (IsKeyPressed(KEY_THREE)) {
            TransformTo(PlayerMode::TRUCK, sounds, particles);
        } else if (IsKeyPressed(KEY_C) || IsKeyPressed(KEY_LEFT_SHIFT)) {
            CycleMode(sounds, particles);
        }
    }

    // Horizontal Movement
    float moveX = 0.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) moveX -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) moveX += 1.0f;

    if (moveX != 0.0f) {
        m_facing = (moveX > 0.0f) ? 1.0f : -1.0f;
    }

    // JETPACK AERIAL MODE FLIGHT CONTROLS
    if (m_jetpackMode) {
        float flySpeed = 380.0f;
        m_vel.x = moveX * flySpeed;

        float moveY = 0.0f;
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_SPACE)) moveY -= 1.0f;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) moveY += 1.0f;

        m_vel.y = moveY * flySpeed;

        // Give the energy-consuming overload priority over the held primary
        // fire input so it remains usable while the player is firing.
        if ((IsKeyPressed(KEY_K) || IsKeyPressed(KEY_X)) && m_blasterCooldown <= 0.0f) {
            InitiateSpecial(sounds, particles, outProjectiles);
        }
        // Hold J/Z for continuous plasma fire. The cooldown controls the
        // firing cadence so this remains frame-rate independent.
        if ((IsKeyDown(KEY_J) || IsKeyDown(KEY_Z)) && m_blasterCooldown <= 0.0f) {
            InitiateMeleeAttack(sounds, particles, outProjectiles);
        }
        return;
    }

    // GROUND MODES:
    if (m_mode == PlayerMode::TRUCK) {
        // Truck physics
        if (moveX != 0.0f) {
            float targetSpeed = moveX * (m_isBoosting ? Constants::TRUCK_MAX_SPEED : 520.0f);
            float accel = Constants::TRUCK_ACCEL * (m_isBoosting ? 1.6f : 1.0f);
            if (targetSpeed > m_vel.x) {
                m_vel.x = std::min(targetSpeed, m_vel.x + accel * GetFrameTime());
            } else {
                m_vel.x = std::max(targetSpeed, m_vel.x - accel * GetFrameTime());
            }
        } else {
            if (m_vel.x > 0.0f) {
                m_vel.x = std::max(0.0f, m_vel.x - Constants::TRUCK_DECEL * GetFrameTime());
            } else if (m_vel.x < 0.0f) {
                m_vel.x = std::min(0.0f, m_vel.x + Constants::TRUCK_DECEL * GetFrameTime());
            }
        }

        // Turbo Boost
        if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_K) || IsKeyPressed(KEY_X)) && m_energy >= 25.0f && !m_isBoosting) {
            m_isBoosting = true;
            m_boostTimer = 1.2f;
            m_energy -= 25.0f;
            sounds.Play(SoundType::TRUCK_RAM, 1.2f, 1.0f);
            particles.EmitSparks({ m_pos.x - m_facing * 40.0f, m_pos.y - 15.0f }, 20, Constants::AUTOBOT_RED, 200.0f, 500.0f);
        }

        // Air horn
        if (IsKeyPressed(KEY_H) || IsKeyPressed(KEY_J)) {
            sounds.Play(SoundType::TRUCK_HORN, 1.0f, 1.0f);
        }
    } else {
        // Robot Modes: Energon Blade & Plasma Cannon
        if (moveX != 0.0f) {
            float targetSpeed = moveX * Constants::ROBOT_MOVE_SPEED;
            if (targetSpeed > m_vel.x) {
                m_vel.x = std::min(targetSpeed, m_vel.x + Constants::ROBOT_ACCEL * GetFrameTime());
            } else {
                m_vel.x = std::max(targetSpeed, m_vel.x - Constants::ROBOT_ACCEL * GetFrameTime());
            }
        } else {
            if (m_vel.x > 0.0f) {
                m_vel.x = std::max(0.0f, m_vel.x - Constants::ROBOT_DECEL * GetFrameTime());
            } else if (m_vel.x < 0.0f) {
                m_vel.x = std::min(0.0f, m_vel.x + Constants::ROBOT_DECEL * GetFrameTime());
            }
        }

        // Jump & Double-jump
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP)) {
            if (m_isGrounded) {
                m_vel.y = Constants::ROBOT_JUMP_FORCE;
                m_isGrounded = false;
                sounds.Play(SoundType::SLASH_LIGHT, 1.4f, 0.6f);
            } else if (m_canDoubleJump && m_mode == PlayerMode::ENERGON_BLADE) {
                m_vel.y = Constants::ROBOT_JUMP_FORCE * 0.90f;
                m_canDoubleJump = false;
                sounds.Play(SoundType::TRANSFORM, 1.6f, 0.7f);
                particles.EmitSparks(
                    { m_pos.x, m_pos.y - 10.0f },
                    10,
                    Constants::ENERGON_CYAN,
                    100.0f,
                    250.0f
                );
            }
        }

        // Primary Attack
        if (IsKeyPressed(KEY_J) || IsKeyPressed(KEY_Z)) {
            InitiateMeleeAttack(sounds, particles, outProjectiles);
        }

        // Special Attack
        if (IsKeyPressed(KEY_K) || IsKeyPressed(KEY_X)) {
            InitiateSpecial(sounds, particles, outProjectiles);
        }
    }
}

void Player::InitiateMeleeAttack(SoundSystem& sounds, ParticleSystem& particles, std::vector<Projectile>& outProjectiles) {
    if (!m_jetpackMode && m_isAttacking && m_mode == PlayerMode::ENERGON_BLADE) return;

    // Aerial mode must take precedence over the default Energon Blade mode.
    // Otherwise the Jetpack controls trigger ground melee instead of firing.
    if (m_jetpackMode || m_mode == PlayerMode::PLASMA_CANNON) {
        // Fire Ion Blaster bolt while airborne.
        if (m_blasterCooldown > 0.0f) return;
        m_blasterCooldown = 0.18f;

        sounds.Play(SoundType::BLASTER, 1.0f, 0.9f);
        Vector2 muzzle = { m_pos.x + m_facing * 40.0f, m_pos.y - 62.0f };
        Vector2 projVel = { m_facing * 920.0f, 0.0f };
        outProjectiles.emplace_back(
            m_currentAttackId++, Faction::PLAYER, muzzle, projVel,
            ProjectileType::PLAYER_PLASMA_BOLT, 18.0f, 1.8f);
        particles.EmitSparks(muzzle, 6, Constants::ENERGON_CYAN, 80.0f, 220.0f);
        m_vel.x -= m_facing * 60.0f;
    } else if (m_mode == PlayerMode::ENERGON_BLADE) {
        m_isAttacking = true;
        m_currentAttackId++;

        if (m_comboWindowTimer > 0.0f) {
            m_comboStep = (m_comboStep % 3) + 1;
        } else {
            m_comboStep = 1;
        }

        if (m_comboStep == 1) {
            m_attackDuration = 0.22f;
            sounds.Play(SoundType::SLASH_LIGHT, 1.0f, 0.9f);
            m_vel.x += m_facing * 90.0f;
        } else if (m_comboStep == 2) {
            m_attackDuration = 0.24f;
            sounds.Play(SoundType::SLASH_LIGHT, 1.25f, 0.95f);
            m_vel.x += m_facing * 120.0f;
            m_vel.y = -220.0f;
        } else if (m_comboStep == 3) {
            m_attackDuration = 0.35f;
            sounds.Play(SoundType::SLASH_HEAVY, 1.0f, 1.0f);
            m_vel.x += m_facing * 180.0f;
            particles.EmitShockwave(
                { m_pos.x + m_facing * 40.0f, m_pos.y - 20.0f },
                60.0f,
                Constants::ENERGON_ORANGE,
                0.25f,
                4.0f
            );
        }
        m_attackTimer = m_attackDuration;
        m_comboWindowTimer = 0.0f;
    }
}

void Player::InitiateSpecial(SoundSystem& sounds, ParticleSystem& particles, std::vector<Projectile>& outProjectiles) {
    if (m_jetpackMode || m_mode == PlayerMode::PLASMA_CANNON) {
        if (m_energy < 30.0f) return;
        m_energy -= 30.0f;
        m_blasterCooldown = 0.45f;

        sounds.Play(SoundType::EXPLOSION, 1.3f, 1.0f);
        sounds.Play(SoundType::BLASTER, 0.8f, 1.0f);
        Vector2 muzzle = { m_pos.x + m_facing * 42.0f, m_pos.y - 62.0f };
        for (int i = -1; i <= 1; i += 2) {
            Vector2 projVel = { m_facing * 820.0f, (float)i * 120.0f };
            outProjectiles.emplace_back(
                m_currentAttackId++, Faction::PLAYER, muzzle, projVel,
                ProjectileType::PLAYER_PLASMA_BOLT, 32.0f, 2.0f);
        }
        particles.EmitShockwave(muzzle, 75.0f, Constants::ENERGON_CYAN, 0.35f, 5.0f);
        m_vel.x -= m_facing * 140.0f;
    } else if (m_mode == PlayerMode::ENERGON_BLADE) {
        if (m_energy < 25.0f) return;
        m_energy -= 25.0f;

        m_isAttacking = true;
        m_comboStep = 3;
        m_attackDuration = 0.32f;
        m_attackTimer = m_attackDuration;
        m_currentAttackId++;

        sounds.Play(SoundType::SLASH_HEAVY, 1.2f, 1.0f);

        Vector2 wavePos = { m_pos.x + m_facing * 45.0f, m_pos.y - 45.0f };
        Vector2 waveVel = { m_facing * 680.0f, 0.0f };

        outProjectiles.emplace_back(
            m_currentAttackId,
            Faction::PLAYER,
            wavePos,
            waveVel,
            ProjectileType::PLAYER_ENERGON_WAVE,
            45.0f,
            2.2f
        );

        particles.EmitShockwave(wavePos, 80.0f, Constants::ENERGON_CYAN, 0.3f, 4.0f);
    }
}

Hitbox Player::GetMeleeHitbox() const {
    Hitbox hb;
    hb.active = false;
    if (m_jetpackMode || m_mode != PlayerMode::ENERGON_BLADE || !m_isAttacking) return hb;

    float reachX = (m_comboStep == 3) ? 78.0f : 58.0f;
    float reachH = (m_comboStep == 2) ? 95.0f : 70.0f;
    float hitX = (m_facing > 0.0f) ? m_pos.x : (m_pos.x - reachX);
    float hitY = (m_comboStep == 2) ? (m_pos.y - reachH - 15.0f) : (m_pos.y - reachH);

    hb.rect = { hitX, hitY, reachX, reachH };
    hb.faction = Faction::PLAYER;
    hb.damage = (m_comboStep == 1) ? 18.0f : ((m_comboStep == 2) ? 24.0f : 40.0f);
    hb.knockback = { m_facing * ((m_comboStep == 3) ? 450.0f : 240.0f), (m_comboStep == 2) ? -380.0f : -120.0f };
    hb.isCrit = (m_comboStep == 3);
    hb.active = true;
    hb.attackId = m_currentAttackId;
    hb.hitStopDuration = (m_comboStep == 3) ? Constants::HIT_STOP_CRIT : ((m_comboStep == 2) ? Constants::HIT_STOP_HEAVY : Constants::HIT_STOP_LIGHT);
    hb.causesScreenShake = (m_comboStep >= 2);

    return hb;
}

Hitbox Player::GetTruckRamHitbox() const {
    Hitbox hb;
    hb.active = false;
    if (m_mode != PlayerMode::TRUCK) return hb;

    float speed = fabsf(m_vel.x);
    if (speed < 200.0f) return hb;

    float boxW = 40.0f;
    float boxH = 45.0f;
    float boxX = (m_facing > 0.0f) ? (m_pos.x + 20.0f) : (m_pos.x - 20.0f - boxW);
    float boxY = m_pos.y - boxH;

    hb.rect = { boxX, boxY, boxW, boxH };
    hb.faction = Faction::PLAYER;
    hb.damage = Constants::TRUCK_RAM_DAMAGE * (speed / Constants::TRUCK_MAX_SPEED);
    hb.knockback = { m_facing * (450.0f + speed * 0.4f), -200.0f };
    hb.isCrit = m_isBoosting;
    hb.active = true;
    hb.attackId = (int)(m_animTimer * 10.0f);
    hb.hitStopDuration = Constants::HIT_STOP_HEAVY;
    hb.causesScreenShake = true;

    return hb;
}

void Player::Update(float dt) {
    m_animTimer += dt;
    m_energy = std::min(Constants::PLAYER_MAX_ENERGY, m_energy + Constants::PLAYER_ENERGY_REGEN_RATE * dt);

    if (m_hurtTimer > 0.0f) m_hurtTimer -= dt;
    if (m_flashTimer > 0.0f) m_flashTimer -= dt;
    if (m_invulnTimer > 0.0f) m_invulnTimer -= dt;
    if (m_blasterCooldown > 0.0f) m_blasterCooldown -= dt;
    if (m_transformAnimTimer > 0.0f) m_transformAnimTimer -= dt;

    if (m_isBoosting) {
        m_boostTimer -= dt;
        if (m_boostTimer <= 0.0f) {
            m_isBoosting = false;
        }
    }

    if (m_isAttacking) {
        m_attackTimer -= dt;
        if (m_attackTimer <= 0.0f) {
            m_isAttacking = false;
            m_comboWindowTimer = 0.30f;
        }
    } else if (m_comboWindowTimer > 0.0f) {
        m_comboWindowTimer -= dt;
        if (m_comboWindowTimer <= 0.0f) {
            m_comboStep = 0;
        }
    }

    // Jetpack Flight Mode Physics (Zero Gravity)
    if (m_jetpackMode) {
        m_pos.x += m_vel.x * dt;
        m_pos.y += m_vel.y * dt;
        m_isGrounded = false;

        // Vertical boundary clamps
        if (m_pos.y < 120.0f) m_pos.y = 120.0f;
        if (m_pos.y > Constants::GROUND_Y) m_pos.y = Constants::GROUND_Y;

        // Horizontal boundary clamps
        if (m_pos.x - m_width * 0.5f < Constants::ARENA_MIN_X) {
            m_pos.x = Constants::ARENA_MIN_X + m_width * 0.5f;
            m_vel.x = 0.0f;
        } else if (m_pos.x + m_width * 0.5f > Constants::ARENA_MAX_X) {
            m_pos.x = Constants::ARENA_MAX_X - m_width * 0.5f;
            m_vel.x = 0.0f;
        }
        return;
    }

    // Standard Ground Mode Vertical Physics & Gravity
    if (m_pos.y < Constants::GROUND_Y) {
        m_isGrounded = false;
    } else {
        m_pos.y = Constants::GROUND_Y;
        if (m_vel.y > 0.0f) {
            m_vel.y = 0.0f;
        }
        m_isGrounded = true;
        m_canDoubleJump = (m_mode == PlayerMode::ENERGON_BLADE);
    }

    if (!m_isGrounded || m_vel.y < 0.0f) {
        m_vel.y += Constants::GRAVITY * dt;
        m_isGrounded = false;
    }

    constexpr float MAX_FALL_SPEED = 1200.0f;
    constexpr float MAX_UPWARD_SPEED = -850.0f;
    if (m_vel.y < MAX_UPWARD_SPEED) m_vel.y = MAX_UPWARD_SPEED;
    if (m_vel.y > MAX_FALL_SPEED) m_vel.y = MAX_FALL_SPEED;

    m_pos.x += m_vel.x * dt;
    m_pos.y += m_vel.y * dt;

    if (m_pos.y >= Constants::GROUND_Y) {
        m_pos.y = Constants::GROUND_Y;
        if (m_vel.y > 0.0f) {
            m_vel.y = 0.0f;
        }
        m_isGrounded = true;
        m_canDoubleJump = (m_mode == PlayerMode::ENERGON_BLADE);
    }

    constexpr float ARENA_CEILING_Y = 90.0f;
    if (m_pos.y - m_height < ARENA_CEILING_Y) {
        m_pos.y = ARENA_CEILING_Y + m_height;
        if (m_vel.y < 0.0f) {
            m_vel.y = 0.0f;
        }
    }

    m_wheelRotation += (m_vel.x * dt * 0.25f);

    if (m_pos.x - m_width * 0.5f < Constants::ARENA_MIN_X) {
        m_pos.x = Constants::ARENA_MIN_X + m_width * 0.5f;
        m_vel.x = 0.0f;
    } else if (m_pos.x + m_width * 0.5f > Constants::ARENA_MAX_X) {
        m_pos.x = Constants::ARENA_MAX_X - m_width * 0.5f;
        m_vel.x = 0.0f;
    }

    // Walk animation cycle
    if (m_isGrounded && fabsf(m_vel.x) > 10.0f) {
        m_walkCycle += dt * (fabsf(m_vel.x) * 0.035f);
    } else {
        m_walkCycle = 0.0f;
    }
}

void Player::DrawJetpackWings(Vector2 center) const {
    float f = m_facing;
    Color wingCol = { 40, 48, 62, 255 };
    Color cyan = Constants::ENERGON_CYAN;

    // Wing struts on back
    Vector2 wingRoot = { center.x - f * 12.0f, center.y - 70.0f };
    Vector2 wingTip1 = { center.x - f * 42.0f, center.y - 105.0f };
    Vector2 wingTip2 = { center.x - f * 34.0f, center.y - 50.0f };

    DrawTriangle(wingRoot, (f > 0.0f) ? wingTip1 : wingTip2, (f > 0.0f) ? wingTip2 : wingTip1, wingCol);
    DrawLineEx(wingRoot, wingTip1, 2.5f, cyan);

    // Rocket Thruster Bell
    Vector2 thrusterPos = { center.x - f * 18.0f, center.y - 55.0f };
    DrawRectangle((int)(thrusterPos.x - 5), (int)thrusterPos.y, 10, 14, DARKGRAY);

    // Blazing Rocket Flame Trails
    float flicker = sinf(m_animTimer * 28.0f) * 4.0f;
    DrawCircle((int)thrusterPos.x, (int)(thrusterPos.y + 16.0f), 6.0f + flicker * 0.5f, cyan);
    DrawCircle((int)thrusterPos.x, (int)(thrusterPos.y + 14.0f), 3.0f, RAYWHITE);
}

void Player::DrawRobot(Vector2 drawPos) const {
    float f = m_facing;
    float walkSin = sinf(m_walkCycle);
    float bobY = fabsf(walkSin) * 4.0f;

    if (m_flashTimer > 0.0f && ((int)(m_flashTimer * 30.0f) % 2 == 0)) {
        DrawRectangleRounded({ drawPos.x - 25.0f, drawPos.y - 92.0f, 50.0f, 92.0f }, 0.3f, 4, RAYWHITE);
        return;
    }

    // Jetpack Wings & Thruster if in aerial flight mode
    if (m_jetpackMode) {
        DrawJetpackWings(drawPos);
    }

    Color blue = Constants::AUTOBOT_BLUE;
    Color darkBlue = { 18, 48, 140, 255 };
    Color red = Constants::AUTOBOT_RED;
    Color chrome = { 205, 215, 230, 255 };
    Color silver = { 170, 180, 195, 255 };

    // 1. Dual Chrome Smokestacks on Upper Back (mirror with f)
    float stackX1 = drawPos.x - f * 14.0f;
    float stackX2 = drawPos.x - f * 8.0f;
    DrawRectangle((int)(stackX1 - 2.5f), (int)(drawPos.y - 96.0f - bobY), 5, 46, chrome);
    DrawRectangle((int)(stackX2 - 2.5f), (int)(drawPos.y - 92.0f - bobY), 5, 42, chrome);

    // 2. Legs & Boots (Properly mirrored left/right facing)
    float legOffset = walkSin * 10.0f;

    // BACK LEG
    float backLegX = drawPos.x - f * 8.0f - f * legOffset * 0.7f;
    // Thigh
    DrawRectangle((int)(backLegX - 5.0f), (int)(drawPos.y - 44.0f), 10, 18, { 140, 145, 155, 255 });
    // Shin
    DrawRectangle((int)(backLegX - 6.0f), (int)(drawPos.y - 28.0f), 12, 18, darkBlue);
    // Foot
    float backFootX = (f > 0.0f) ? (backLegX - 6.0f) : (backLegX - 14.0f);
    DrawRectangle((int)backFootX, (int)(drawPos.y - 10.0f), 20, 10, darkBlue);

    // FRONT LEG
    float frontLegX = drawPos.x + f * 4.0f + f * legOffset * 0.7f;
    // Thigh (silver)
    DrawRectangle((int)(frontLegX - 6.0f), (int)(drawPos.y - 46.0f), 12, 20, silver);
    // Shin (blue Autobot armor)
    DrawRectangle((int)(frontLegX - 7.0f), (int)(drawPos.y - 28.0f), 14, 18, blue);
    // Silver knee guard
    DrawRectangle((int)(frontLegX + f * 2.0f - 4.0f), (int)(drawPos.y - 30.0f), 8, 6, chrome);
    // Boot: toe correctly faces direction f
    float frontFootX = (f > 0.0f) ? (frontLegX - 6.0f) : (frontLegX - 16.0f);
    DrawRectangle((int)frontFootX, (int)(drawPos.y - 10.0f), 22, 10, blue);
    // Yellow toe cap
    float toeX = (f > 0.0f) ? (frontFootX + 16.0f) : frontFootX;
    DrawRectangle((int)toeX, (int)(drawPos.y - 9.0f), 6, 9, { 240, 200, 30, 255 });

    // 3. Pelvis & Belt (Segmented silver with yellow crest)
    DrawRectangle((int)(drawPos.x - 14.0f), (int)(drawPos.y - 48.0f - bobY), 28, 9, silver);
    DrawRectangle((int)(drawPos.x - 5.0f), (int)(drawPos.y - 47.0f - bobY), 10, 7, { 240, 200, 20, 255 });

    // 4. Torso & Cab Chest
    DrawRectangleRounded({ drawPos.x - 22.0f, drawPos.y - 80.0f - bobY, 44.0f, 32.0f }, 0.2f, 4, red);
    DrawRectangleLines((int)(drawPos.x - 22.0f), (int)(drawPos.y - 80.0f - bobY), 44, 32, { 160, 20, 28, 255 });

    // Silver Grille & Bumper in abdomen
    DrawRectangle((int)(drawPos.x - 10.0f), (int)(drawPos.y - 58.0f - bobY), 20, 9, chrome);
    DrawLine((int)(drawPos.x - 10.0f), (int)(drawPos.y - 54.0f - bobY), (int)(drawPos.x + 10.0f), (int)(drawPos.y - 54.0f - bobY), DARKGRAY);

    // Twin Blue Windshield Windows
    Color glassBlue = { 80, 190, 245, 230 };
    DrawRectangle((int)(drawPos.x - 18.0f), (int)(drawPos.y - 76.0f - bobY), 16, 14, glassBlue);
    DrawRectangle((int)(drawPos.x + 2.0f), (int)(drawPos.y - 76.0f - bobY), 16, 14, glassBlue);
    DrawRectangleLines((int)(drawPos.x - 18.0f), (int)(drawPos.y - 76.0f - bobY), 16, 14, chrome);
    DrawRectangleLines((int)(drawPos.x + 2.0f), (int)(drawPos.y - 76.0f - bobY), 16, 14, chrome);

    // Glowing Central AllSpark / Matrix Core in windshield center
    DrawCircle((int)drawPos.x, (int)(drawPos.y - 68.0f - bobY), 3.5f, Constants::ENERGON_CYAN);
    DrawCircle((int)drawPos.x, (int)(drawPos.y - 68.0f - bobY), 1.5f, RAYWHITE);

    // Autobot golden crest
    DrawTriangle(
        { drawPos.x - 4.0f, drawPos.y - 70.0f - bobY },
        { drawPos.x, drawPos.y - 64.0f - bobY },
        { drawPos.x + 4.0f, drawPos.y - 70.0f - bobY },
        { 255, 220, 40, 255 }
    );

    // 5. Head / Helmet (Iconic Optimus Prime helmet)
    DrawRectangle((int)(drawPos.x - 9.0f), (int)(drawPos.y - 96.0f - bobY), 18, 16, blue);
    // Silver side antenna spikes
    DrawRectangle((int)(drawPos.x - 12.0f), (int)(drawPos.y - 101.0f - bobY), 3, 16, chrome);
    DrawRectangle((int)(drawPos.x + 9.0f), (int)(drawPos.y - 101.0f - bobY), 3, 16, chrome);
    // Tall central silver fin crest
    DrawTriangle(
        { drawPos.x - 2.5f, drawPos.y - 96.0f - bobY },
        { drawPos.x, drawPos.y - 104.0f - bobY },
        { drawPos.x + 2.5f, drawPos.y - 96.0f - bobY },
        chrome
    );
    // Silver faceplate
    DrawRectangle((int)(drawPos.x - 6.0f + f * 2.0f), (int)(drawPos.y - 87.0f - bobY), 12, 7, chrome);
    // Glowing Cyan Visor / Optics
    DrawRectangle((int)(drawPos.x - 5.0f + f * 3.0f), (int)(drawPos.y - 92.0f - bobY), 10, 3, Constants::ENERGON_CYAN);

    // 6. Shoulders & Arms
    DrawRectangleRounded({ drawPos.x - f * 24.0f - 4.0f, drawPos.y - 80.0f - bobY, 14.0f, 14.0f }, 0.4f, 4, darkBlue);
    DrawRectangleRounded({ drawPos.x + f * 14.0f - 4.0f, drawPos.y - 80.0f - bobY, 16.0f, 16.0f }, 0.4f, 4, red);

    Vector2 weaponHandPos = { drawPos.x + f * 22.0f, drawPos.y - 64.0f - bobY };

    if (m_mode == PlayerMode::ENERGON_BLADE && !m_jetpackMode) {
        DrawEnergonBlade(weaponHandPos);
        if (m_isAttacking) {
            DrawMeleeSlashArc(drawPos);
        }
    } else {
        DrawPlasmaCannon(weaponHandPos);
    }
}

void Player::DrawEnergonBlade(Vector2 handPos) const {
    float f = m_facing;
    Color bladeCol = (m_comboStep == 3) ? Constants::ENERGON_ORANGE : Constants::ENERGON_CYAN;

    Vector2 bladeBase = handPos;
    Vector2 bladeTip = { handPos.x + f * 44.0f, handPos.y + (m_isAttacking ? 10.0f : -15.0f) };

    DrawLineEx(bladeBase, bladeTip, 10.0f, { bladeCol.r, bladeCol.g, bladeCol.b, 90 });
    DrawLineEx(bladeBase, bladeTip, 5.0f, bladeCol);
    DrawLineEx(bladeBase, bladeTip, 2.0f, RAYWHITE);

    Vector2 axeWing = { bladeTip.x - f * 12.0f, bladeTip.y - 20.0f };
    DrawTriangle(bladeTip, axeWing, { bladeBase.x + f * 15.0f, bladeBase.y - 8.0f }, bladeCol);
}

void Player::DrawPlasmaCannon(Vector2 armPos) const {
    float f = m_facing;
    Color gunMetal = { 45, 52, 65, 255 };
    Color darkSteel = { 25, 28, 35, 255 };
    Color chrome = { 200, 215, 235, 255 };
    Color cyan = Constants::ENERGON_CYAN;

    float cannonW = 38.0f;
    float cannonH = 14.0f;
    float minX = (f > 0.0f) ? armPos.x : (armPos.x - cannonW);
    float minY = armPos.y - 7.0f;

    // Solid opaque cannon body
    DrawRectangle((int)minX, (int)minY, (int)cannonW, (int)cannonH, gunMetal);
    DrawRectangleLines((int)minX, (int)minY, (int)cannonW, (int)cannonH, darkSteel);

    // Chrome radiator shield on top
    DrawRectangle((int)minX + 4, (int)minY - 2, (int)cannonW - 8, 4, chrome);

    // Glowing energy coil rings
    float coilX1 = (f > 0.0f) ? (minX + 8.0f) : (minX + 22.0f);
    float coilX2 = (f > 0.0f) ? (minX + 18.0f) : (minX + 12.0f);
    DrawRectangle((int)coilX1, (int)minY + 2, 5, (int)cannonH - 4, cyan);
    DrawRectangle((int)coilX2, (int)minY + 2, 5, (int)cannonH - 4, cyan);

    // Brilliant white muzzle
    float muzzleX = (f > 0.0f) ? (minX + cannonW - 5.0f) : minX;
    DrawRectangle((int)muzzleX, (int)minY - 1, 5, (int)cannonH + 2, RAYWHITE);
    DrawCircle((int)(muzzleX + (f > 0.0f ? 5.0f : 0.0f)), (int)(minY + cannonH * 0.5f), 4.0f, cyan);
}

void Player::DrawMeleeSlashArc(Vector2 center) const {
    float f = m_facing;
    float progress = 1.0f - (m_attackTimer / m_attackDuration);
    Color arcCol = (m_comboStep == 3) ? Constants::ENERGON_ORANGE : Constants::ENERGON_CYAN;
    arcCol.a = (unsigned char)(220.0f * (1.0f - progress));

    float startAngle = (m_comboStep == 2) ? 60.0f : -80.0f;
    float sweep = (m_comboStep == 2) ? -130.0f : 150.0f;
    float curAngle = startAngle + sweep * progress;

    Vector2 arcCenter = { center.x + f * 15.0f, center.y - 50.0f };
    float radius = 55.0f;

    DrawCircleSector(arcCenter, radius, curAngle - 35.0f, curAngle, 16, arcCol);
    DrawCircleSectorLines(arcCenter, radius, curAngle - 35.0f, curAngle, 16, RAYWHITE);
}

void Player::DrawTruck(Vector2 drawPos) const {
    float f = m_facing;
    Color red = Constants::AUTOBOT_RED;
    Color blue = Constants::AUTOBOT_BLUE;
    Color chrome = { 210, 220, 235, 255 };

    if (m_flashTimer > 0.0f && ((int)(m_flashTimer * 30.0f) % 2 == 0)) {
        DrawRectangleRounded({ drawPos.x - 55.0f, drawPos.y - 52.0f, 110.0f, 52.0f }, 0.2f, 4, RAYWHITE);
        return;
    }

    float leftX = drawPos.x - 55.0f;
    float topY = drawPos.y - 50.0f;

    // 1. Blue Rear Bed / Frame
    DrawRectangle((int)leftX, (int)(topY + 14.0f), 52, 24, blue);
    DrawRectangleLines((int)leftX, (int)(topY + 14.0f), 52, 24, { 18, 48, 140, 255 });

    // 2. Red Front Cab (Classic Cab-over-engine)
    DrawRectangleRounded({ leftX + 48.0f, topY, 62.0f, 38.0f }, 0.15f, 4, red);
    DrawRectangleLines((int)(leftX + 48.0f), (int)topY, 62, 38, { 160, 20, 28, 255 });

    // Dual vertical chrome exhaust stacks
    DrawRectangle((int)(leftX + 44.0f), (int)(topY - 18.0f), 6, 36, chrome);
    DrawRectangle((int)(leftX + 40.0f), (int)(topY - 14.0f), 5, 32, chrome);

    // Chrome front grille
    float grilleX = (f > 0.0f) ? (leftX + 104.0f) : (leftX - 4.0f);
    DrawRectangle((int)grilleX, (int)(topY + 12.0f), 8, 22, chrome);
    DrawRectangleLines((int)grilleX, (int)(topY + 12.0f), 8, 22, DARKGRAY);

    // Blue Windshield on Cab
    float windX = (f > 0.0f) ? (leftX + 80.0f) : (leftX + 56.0f);
    DrawRectangle((int)windX, (int)(topY + 4.0f), 24, 12, { 80, 190, 245, 230 });
    DrawRectangleLines((int)windX, (int)(topY + 4.0f), 24, 12, chrome);

    // Front Headlights & Light Beam Cone
    float headX = (f > 0.0f) ? (leftX + 110.0f) : leftX;
    DrawCircle((int)headX, (int)(topY + 28.0f), 5.0f, { 255, 240, 120, 255 });
    Vector2 coneApex = { headX, topY + 28.0f };
    Vector2 coneTop = { headX + f * 120.0f, topY + 10.0f };
    Vector2 coneBottom = { headX + f * 120.0f, topY + 45.0f };
    DrawTriangle(coneApex, (f > 0.0f) ? coneBottom : coneTop, (f > 0.0f) ? coneTop : coneBottom, { 255, 245, 160, 45 });

    // 3. Rotating Rubber Wheels
    float wheelRadius = 11.0f;
    float wheelY = drawPos.y - wheelRadius;
    float wheelXs[3] = { leftX + 14.0f, leftX + 38.0f, leftX + 94.0f };

    for (int w = 0; w < 3; w++) {
        float wx = wheelXs[w];
        DrawCircle((int)wx, (int)wheelY, wheelRadius, { 25, 28, 35, 255 });
        DrawCircle((int)wx, (int)wheelY, 6.0f, chrome);
        float spokeAngle = m_wheelRotation;
        DrawLine(
            (int)(wx + cosf(spokeAngle) * 5.0f), (int)(wheelY + sinf(spokeAngle) * 5.0f),
            (int)(wx - cosf(spokeAngle) * 5.0f), (int)(wheelY - sinf(spokeAngle) * 5.0f),
            DARKGRAY
        );
    }
}

void Player::Draw() const {
    if (m_isDead) return;

    if (m_mode == PlayerMode::TRUCK) {
        DrawTruck(m_pos);
    } else {
        DrawRobot(m_pos);
    }
}
