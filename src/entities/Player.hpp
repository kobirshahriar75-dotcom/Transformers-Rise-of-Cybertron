#pragma once

#include "Entity.hpp"
#include "Projectile.hpp"
#include "../core/Constants.hpp"
#include "../core/SoundSystem.hpp"
#include "../effects/ParticleSystem.hpp"
#include <vector>

class Player : public Entity {
public:
    Player(int id, Vector2 pos);

    void Update(float dt) override;
    void Draw() const override;
    void HandleInput(SoundSystem& sounds, ParticleSystem& particles, std::vector<Projectile>& outProjectiles);

    void TransformTo(PlayerMode newMode, SoundSystem& sounds, ParticleSystem& particles);
    void CycleMode(SoundSystem& sounds, ParticleSystem& particles);

    // Jetpack Aerial Mode (Levels 7-8 / Aerial Combat)
    void SetJetpackMode(bool enabled) { m_jetpackMode = enabled; }
    bool IsJetpackMode() const { return m_jetpackMode; }

    // Combat
    void InitiateMeleeAttack(SoundSystem& sounds, ParticleSystem& particles, std::vector<Projectile>& outProjectiles);
    void InitiateSpecial(SoundSystem& sounds, ParticleSystem& particles, std::vector<Projectile>& outProjectiles);
    Hitbox GetMeleeHitbox() const;
    Hitbox GetTruckRamHitbox() const;

    // State Accessors
    PlayerMode GetMode() const { return m_mode; }
    float GetEnergy() const { return m_energy; }
    float GetMaxEnergy() const { return Constants::PLAYER_MAX_ENERGY; }
    void AddEnergy(float amount) { m_energy = std::min(Constants::PLAYER_MAX_ENERGY, m_energy + amount); }
    bool IsAttacking() const { return m_isAttacking; }
    int GetComboStep() const { return m_comboStep; }
    bool IsTruckBoosting() const { return m_isBoosting; }

    void Reset(Vector2 pos);

private:
    PlayerMode m_mode;
    float m_energy;
    bool m_jetpackMode;

    // Movement & Animation
    float m_animTimer;
    float m_walkCycle;
    bool m_canDoubleJump;
    bool m_isBoosting;
    float m_boostTimer;
    float m_wheelRotation;

    // Attack state
    bool m_isAttacking;
    int m_comboStep;         // 1, 2, or 3
    float m_attackTimer;
    float m_attackDuration;
    float m_comboWindowTimer;
    int m_currentAttackId;
    float m_blasterCooldown;

    // Transformation visual transition
    float m_transformAnimTimer;

    // Procedural Rendering Helpers
    void DrawRobot(Vector2 drawPos) const;
    void DrawTruck(Vector2 drawPos) const;
    void DrawEnergonBlade(Vector2 handPos) const;
    void DrawPlasmaCannon(Vector2 armPos) const;
    void DrawMeleeSlashArc(Vector2 center) const;
    void DrawJetpackWings(Vector2 center) const;
};
