#include "ParticleSystem.hpp"
#include "../core/Constants.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace {
    inline float RandF() {
        return (float)rand() / (float)RAND_MAX;
    }
}

ParticleSystem::ParticleSystem() {
    m_particles.reserve(512);
    m_rings.reserve(64);
    m_damageTexts.reserve(64);
}

void ParticleSystem::Reset() {
    m_particles.clear();
    m_rings.clear();
    m_damageTexts.clear();
}

void ParticleSystem::EmitSparks(Vector2 pos, int count, Color color, float speedMin, float speedMax) {
    for (int i = 0; i < count; i++) {
        float angle = RandF() * 2.0f * 3.14159f;
        float speed = speedMin + RandF() * (speedMax - speedMin);
        Particle p;
        p.pos = pos;
        p.vel = { cosf(angle) * speed, sinf(angle) * speed - 60.0f };
        p.color = color;
        p.size = 2.0f + RandF() * 3.0f;
        p.life = 0.25f + RandF() * 0.35f;
        p.maxLife = p.life;
        p.affectedByGravity = true;
        m_particles.push_back(p);
    }
}

void ParticleSystem::EmitSmoke(Vector2 pos, Vector2 baseVel, int count, Color color, float size) {
    for (int i = 0; i < count; i++) {
        Particle p;
        p.pos = { pos.x + (RandF() * 12.0f - 6.0f), pos.y + (RandF() * 12.0f - 6.0f) };
        p.vel = { baseVel.x + (RandF() * 50.0f - 25.0f), baseVel.y - 30.0f - RandF() * 50.0f };
        p.color = color;
        p.size = size * (0.8f + RandF() * 0.5f);
        p.life = 0.4f + RandF() * 0.3f;
        p.maxLife = p.life;
        p.affectedByGravity = false;
        m_particles.push_back(p);
    }
}

void ParticleSystem::EmitEnergonSplatter(Vector2 pos, int count, Color color) {
    for (int i = 0; i < count; i++) {
        float angle = RandF() * 2.0f * 3.14159f;
        float speed = 80.0f + RandF() * 220.0f;
        Particle p;
        p.pos = pos;
        p.vel = { cosf(angle) * speed, sinf(angle) * speed - 100.0f };
        p.color = color;
        p.size = 3.5f + RandF() * 4.0f;
        p.life = 0.35f + RandF() * 0.3f;
        p.maxLife = p.life;
        p.affectedByGravity = true;
        m_particles.push_back(p);
    }
}

void ParticleSystem::EmitShockwave(Vector2 pos, float maxRadius, Color color, float duration, float thickness) {
    ShockwaveRing r;
    r.pos = pos;
    r.radius = 4.0f;
    r.maxRadius = maxRadius;
    r.life = duration;
    r.maxLife = duration;
    r.color = color;
    r.thickness = thickness;
    m_rings.push_back(r);
}

void ParticleSystem::EmitDamageText(Vector2 pos, int damage, bool isCrit) {
    DamageText dt;
    dt.pos = { pos.x + (RandF() * 20.0f - 10.0f), pos.y - 20.0f };
    dt.text = (isCrit ? "CRIT! " : "") + std::to_string(damage);
    dt.color = isCrit ? Constants::ENERGON_ORANGE : (damage > 25 ? Constants::AUTOBOT_RED : RAYWHITE);
    dt.life = 0.65f;
    dt.maxLife = 0.65f;
    dt.fontSize = isCrit ? 22.0f : 17.0f;
    m_damageTexts.push_back(dt);
}

void ParticleSystem::Update(float dt) {
    // Update Particles
    for (size_t i = 0; i < m_particles.size(); ) {
        Particle& p = m_particles[i];
        p.life -= dt;
        if (p.life <= 0.0f) {
            m_particles[i] = m_particles.back();
            m_particles.pop_back();
            continue;
        }

        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;

        if (p.affectedByGravity) {
            p.vel.y += 900.0f * dt;
            // Floor bounce
            if (p.pos.y >= Constants::GROUND_Y) {
                p.pos.y = Constants::GROUND_Y;
                p.vel.y = -p.vel.y * 0.45f;
                p.vel.x *= 0.65f;
            }
        }
        i++;
    }

    // Update Shockwave Rings
    for (size_t i = 0; i < m_rings.size(); ) {
        ShockwaveRing& r = m_rings[i];
        r.life -= dt;
        if (r.life <= 0.0f) {
            m_rings[i] = m_rings.back();
            m_rings.pop_back();
            continue;
        }

        float progress = 1.0f - (r.life / r.maxLife);
        r.radius = 4.0f + (r.maxRadius - 4.0f) * progress;
        i++;
    }

    // Update Damage Text popups
    for (size_t i = 0; i < m_damageTexts.size(); ) {
        DamageText& dtObj = m_damageTexts[i];
        dtObj.life -= dt;
        if (dtObj.life <= 0.0f) {
            m_damageTexts[i] = m_damageTexts.back();
            m_damageTexts.pop_back();
            continue;
        }

        dtObj.pos.y -= 50.0f * dt; // float upwards
        i++;
    }
}

void ParticleSystem::Draw() const {
    // Draw Particles
    for (const auto& p : m_particles) {
        float alpha = p.life / p.maxLife;
        Color col = p.color;
        col.a = (unsigned char)(col.a * alpha);

        if (p.affectedByGravity) {
            // Spark / crystal diamond or rect
            DrawRectanglePro(
                { p.pos.x, p.pos.y, p.size, p.size },
                { p.size * 0.5f, p.size * 0.5f },
                p.vel.x * 0.2f,
                col
            );
        } else {
            // Expanding smoke circle
            float currentSize = p.size * (1.0f + (1.0f - alpha) * 0.8f);
            DrawCircleV(p.pos, currentSize, col);
        }
    }

    // Draw Shockwave Rings
    for (const auto& r : m_rings) {
        float alpha = r.life / r.maxLife;
        Color col = r.color;
        col.a = (unsigned char)(255.0f * alpha);
        DrawRing(r.pos, r.radius - r.thickness, r.radius, 0.0f, 360.0f, 32, col);
    }

    // Draw Floating Damage Text
    for (const auto& dtObj : m_damageTexts) {
        float alpha = dtObj.life / dtObj.maxLife;
        Color col = dtObj.color;
        col.a = (unsigned char)(255.0f * alpha);

        // Drop shadow for crisp readability
        Color shadow = BLACK;
        shadow.a = (unsigned char)(180.0f * alpha);
        DrawText(dtObj.text.c_str(), (int)dtObj.pos.x + 1, (int)dtObj.pos.y + 1, (int)dtObj.fontSize, shadow);
        DrawText(dtObj.text.c_str(), (int)dtObj.pos.x, (int)dtObj.pos.y, (int)dtObj.fontSize, col);
    }
}
