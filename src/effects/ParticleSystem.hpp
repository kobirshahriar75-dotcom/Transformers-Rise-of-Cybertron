#pragma once

#include "raylib.h"
#include <vector>
#include <string>

struct Particle {
    Vector2 pos;
    Vector2 vel;
    Color color;
    float size;
    float life;
    float maxLife;
    bool affectedByGravity;
};

struct ShockwaveRing {
    Vector2 pos;
    float radius;
    float maxRadius;
    float life;
    float maxLife;
    Color color;
    float thickness;
};

struct DamageText {
    Vector2 pos;
    std::string text;
    Color color;
    float life;
    float maxLife;
    float fontSize;
};

class ParticleSystem {
public:
    ParticleSystem();

    void Update(float dt);
    void Draw() const;
    void Reset();

    // Particle Spawners
    void EmitSparks(Vector2 pos, int count, Color color, float speedMin = 100.0f, float speedMax = 350.0f);
    void EmitSmoke(Vector2 pos, Vector2 baseVel, int count, Color color, float size = 8.0f);
    void EmitEnergonSplatter(Vector2 pos, int count, Color color);
    void EmitShockwave(Vector2 pos, float maxRadius, Color color, float duration = 0.25f, float thickness = 3.5f);
    void EmitDamageText(Vector2 pos, int damage, bool isCrit = false);

private:
    std::vector<Particle> m_particles;
    std::vector<ShockwaveRing> m_rings;
    std::vector<DamageText> m_damageTexts;
};
