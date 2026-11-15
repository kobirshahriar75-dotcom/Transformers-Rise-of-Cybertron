#include "CombatSystem.hpp"
#include <algorithm>

CombatSystem::CombatSystem()
    : m_hitStopTimer(0.0f)
{
}

void CombatSystem::Reset() {
    m_hitStopTimer = 0.0f;
    m_processedHits.clear();
}

bool CombatSystem::HasHit(int attackId, int targetId) const {
    uint32_t key = ((uint32_t)attackId << 16) | (uint32_t)(targetId & 0xFFFF);
    return m_processedHits.find(key) != m_processedHits.end();
}

void CombatSystem::RecordHit(int attackId, int targetId) {
    uint32_t key = ((uint32_t)attackId << 16) | (uint32_t)(targetId & 0xFFFF);
    m_processedHits.insert(key);

    // Keep set from growing unbounded
    if (m_processedHits.size() > 500) {
        m_processedHits.clear();
    }
}

void CombatSystem::UpdateHitStop(float dt) {
    if (m_hitStopTimer > 0.0f) {
        m_hitStopTimer -= dt;
    }
}

void CombatSystem::Update(
    float dt,
    Player& player,
    std::vector<Enemy>& enemies,
    Boss* boss,
    std::vector<Projectile>& projectiles,
    ParticleSystem& particles,
    CameraManager& camera,
    SoundSystem& sounds
) {
    // Decrement hit-stop timer
    if (m_hitStopTimer > 0.0f) {
        m_hitStopTimer -= dt;
    }

    // 1. Check Player Attacks vs Enemies
    std::vector<Hitbox> playerHits;
    Hitbox meleeHb = player.GetMeleeHitbox();
    if (meleeHb.active) playerHits.push_back(meleeHb);

    Hitbox ramHb = player.GetTruckRamHitbox();
    if (ramHb.active) playerHits.push_back(ramHb);

    for (const auto& hb : playerHits) {
        // Against standard enemies
        for (auto& enemy : enemies) {
            if (enemy.IsDead()) continue;

            if (!HasHit(hb.attackId, enemy.GetId()) && CheckCollisionRecs(hb.rect, enemy.GetHurtbox().rect)) {
                RecordHit(hb.attackId, enemy.GetId());
                enemy.TakeDamage(hb.damage, hb.knockback, hb.isCrit);

                // Feedback: Sounds & Juice
                sounds.Play(SoundType::IMPACT_METAL, 1.0f + ((float)(rand() % 10) / 50.0f), 0.95f);
                if (hb.isCrit) sounds.Play(SoundType::TRUCK_RAM, 1.1f, 0.85f);

                // Energy reward for player on hit
                player.AddEnergy(6.0f);

                // Effects
                Vector2 impactPt = {
                    (hb.rect.x + enemy.GetBounds().x + enemy.GetBounds().width * 0.5f) * 0.5f,
                    (hb.rect.y + enemy.GetBounds().y + enemy.GetBounds().height * 0.5f) * 0.5f
                };

                particles.EmitSparks(impactPt, hb.isCrit ? 18 : 10, Constants::AUTOBOT_RED, 120.0f, 320.0f);
                particles.EmitDamageText(impactPt, (int)hb.damage, hb.isCrit);
                particles.EmitShockwave(impactPt, hb.isCrit ? 50.0f : 30.0f, Constants::ENERGON_ORANGE, 0.20f, 3.0f);

                // Trigger Hit-Stop & Screen Shake
                m_hitStopTimer = hb.hitStopDuration;
                if (hb.causesScreenShake) {
                    camera.AddTrauma(hb.isCrit ? 0.45f : 0.25f);
                }
            }
        }

        // Against Boss (Megatron)
        if (boss && !boss->IsDead()) {
            if (!HasHit(hb.attackId, boss->GetId()) && CheckCollisionRecs(hb.rect, boss->GetHurtbox().rect)) {
                RecordHit(hb.attackId, boss->GetId());
                boss->TakeDamage(hb.damage, { hb.knockback.x * 0.4f, hb.knockback.y * 0.3f }, hb.isCrit);

                sounds.Play(SoundType::IMPACT_METAL, 0.9f, 1.0f);
                player.AddEnergy(8.0f);

                Vector2 impactPt = {
                    (hb.rect.x + boss->GetBounds().x + boss->GetBounds().width * 0.5f) * 0.5f,
                    (hb.rect.y + boss->GetBounds().y + boss->GetBounds().height * 0.5f) * 0.5f
                };

                particles.EmitSparks(impactPt, hb.isCrit ? 22 : 12, Constants::ENERGON_PURPLE, 140.0f, 360.0f);
                particles.EmitDamageText(impactPt, (int)hb.damage, hb.isCrit);
                particles.EmitShockwave(impactPt, 60.0f, Constants::ENERGON_CYAN, 0.25f, 4.0f);

                m_hitStopTimer = hb.hitStopDuration * 1.2f;
                camera.AddTrauma(hb.isCrit ? 0.55f : 0.30f);
            }
        }
    }

    // 2. Check Projectiles vs Targets
    for (auto& proj : projectiles) {
        if (proj.IsDead()) continue;
        Hitbox pHb = proj.GetHitbox();

        if (pHb.faction == Faction::PLAYER) {
            // Player projectile vs Enemies
            for (auto& enemy : enemies) {
                if (enemy.IsDead()) continue;

                if (!HasHit(pHb.attackId, enemy.GetId()) && CheckCollisionRecs(pHb.rect, enemy.GetHurtbox().rect)) {
                    RecordHit(pHb.attackId, enemy.GetId());
                    enemy.TakeDamage(pHb.damage, pHb.knockback, pHb.isCrit);
                    proj.TakeDamage(10.0f, { 0.0f, 0.0f }); // Destroys projectile unless piercing

                    sounds.Play(SoundType::EXPLOSION, 1.3f, 0.65f);
                    Vector2 pPos = proj.GetPosition();
                    particles.EmitSparks(pPos, 14, Constants::ENERGON_CYAN, 120.0f, 280.0f);
                    particles.EmitDamageText(pPos, (int)pHb.damage, pHb.isCrit);
                    particles.EmitShockwave(pPos, 35.0f, Constants::ENERGON_CYAN, 0.20f, 3.0f);

                    m_hitStopTimer = pHb.hitStopDuration;
                    if (pHb.causesScreenShake) camera.AddTrauma(0.35f);
                    break;
                }
            }

            // Player projectile vs Boss
            if (boss && !boss->IsDead()) {
                if (!HasHit(pHb.attackId, boss->GetId()) && CheckCollisionRecs(pHb.rect, boss->GetHurtbox().rect)) {
                    RecordHit(pHb.attackId, boss->GetId());
                    boss->TakeDamage(pHb.damage, { pHb.knockback.x * 0.3f, -40.0f }, pHb.isCrit);
                    proj.TakeDamage(10.0f, { 0.0f, 0.0f });

                    sounds.Play(SoundType::EXPLOSION, 1.2f, 0.8f);
                    Vector2 pPos = proj.GetPosition();
                    particles.EmitSparks(pPos, 16, Constants::ENERGON_CYAN, 140.0f, 320.0f);
                    particles.EmitDamageText(pPos, (int)pHb.damage, pHb.isCrit);
                    particles.EmitShockwave(pPos, 45.0f, Constants::ENERGON_CYAN, 0.25f, 3.5f);

                    m_hitStopTimer = pHb.hitStopDuration;
                    camera.AddTrauma(0.35f);
                }
            }
        } else {
            // Enemy / Boss projectile vs Player
            if (!player.IsDead() && !HasHit(pHb.attackId, player.GetId()) && CheckCollisionRecs(pHb.rect, player.GetHurtbox().rect)) {
                RecordHit(pHb.attackId, player.GetId());
                player.TakeDamage(pHb.damage, pHb.knockback, pHb.isCrit);
                proj.TakeDamage(10.0f, { 0.0f, 0.0f });

                sounds.Play(SoundType::HURT, 1.0f, 1.0f);
                sounds.Play(SoundType::EXPLOSION, 1.1f, 0.7f);

                Vector2 pPos = player.GetPosition();
                particles.EmitSparks(pPos, 16, Constants::DECEPTICON_PURPLE, 120.0f, 300.0f);
                particles.EmitDamageText(pPos, (int)pHb.damage, true);

                m_hitStopTimer = pHb.hitStopDuration;
                camera.AddTrauma(0.40f);
            }
        }
    }

    // 3. Check Enemy Melee vs Player
    for (auto& enemy : enemies) {
        if (enemy.IsDead()) continue;
        Hitbox eHb = enemy.GetMeleeHitbox();
        if (eHb.active && !player.IsDead()) {
            if (!HasHit(eHb.attackId, player.GetId()) && CheckCollisionRecs(eHb.rect, player.GetHurtbox().rect)) {
                RecordHit(eHb.attackId, player.GetId());
                player.TakeDamage(eHb.damage, eHb.knockback, eHb.isCrit);

                sounds.Play(SoundType::HURT, 1.0f, 1.0f);
                Vector2 pPos = player.GetPosition();
                particles.EmitSparks(pPos, 14, Constants::DECEPTICON_PURPLE, 100.0f, 260.0f);
                particles.EmitDamageText(pPos, (int)eHb.damage, eHb.isCrit);

                m_hitStopTimer = eHb.hitStopDuration;
                if (eHb.causesScreenShake) camera.AddTrauma(0.35f);
            }
        }
    }

    // 4. Check Boss Attacks vs Player
    if (boss && !boss->IsDead() && !player.IsDead()) {
        // Mace swing
        Hitbox maceHb = boss->GetMeleeHitbox();
        if (maceHb.active && !HasHit(maceHb.attackId, player.GetId()) && CheckCollisionRecs(maceHb.rect, player.GetHurtbox().rect)) {
            RecordHit(maceHb.attackId, player.GetId());
            player.TakeDamage(maceHb.damage, maceHb.knockback, maceHb.isCrit);

            sounds.Play(SoundType::HURT, 0.9f, 1.0f);
            sounds.Play(SoundType::IMPACT_METAL, 0.8f, 1.0f);

            Vector2 pPos = player.GetPosition();
            particles.EmitSparks(pPos, 22, Constants::ENERGON_PURPLE, 150.0f, 380.0f);
            particles.EmitDamageText(pPos, (int)maceHb.damage, true);
            particles.EmitShockwave(pPos, 65.0f, Constants::ENERGON_PURPLE, 0.30f, 5.0f);

            m_hitStopTimer = maceHb.hitStopDuration;
            camera.AddTrauma(0.60f);
        }

        // Jet dash ram
        Hitbox dashHb = boss->GetDashHitbox();
        if (dashHb.active && !HasHit(dashHb.attackId, player.GetId()) && CheckCollisionRecs(dashHb.rect, player.GetHurtbox().rect)) {
            RecordHit(dashHb.attackId, player.GetId());
            player.TakeDamage(dashHb.damage, dashHb.knockback, dashHb.isCrit);

            sounds.Play(SoundType::TRUCK_RAM, 0.9f, 1.0f);
            sounds.Play(SoundType::HURT, 0.9f, 1.0f);

            Vector2 pPos = player.GetPosition();
            particles.EmitSparks(pPos, 20, Constants::ENERGON_PURPLE, 140.0f, 350.0f);
            particles.EmitDamageText(pPos, (int)dashHb.damage, true);

            m_hitStopTimer = dashHb.hitStopDuration;
            camera.AddTrauma(0.50f);
        }
    }
}
