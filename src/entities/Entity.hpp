#pragma once

#include "raylib.h"
#include "../core/Constants.hpp"
#include "../combat/Hitbox.hpp"
#include <algorithm>

class Entity {
public:
    Entity(int id, Faction faction, Vector2 pos, float width, float height, float maxHp)
        : m_id(id)
        , m_faction(faction)
        , m_pos(pos)
        , m_vel{ 0.0f, 0.0f }
        , m_width(width)
        , m_height(height)
        , m_hp(maxHp)
        , m_maxHp(maxHp)
        , m_facing(1.0f) // 1.0f for right, -1.0f for left
        , m_isGrounded(true)
        , m_isDead(false)
        , m_hurtTimer(0.0f)
        , m_flashTimer(0.0f)
        , m_invulnTimer(0.0f)
    {
    }

    virtual ~Entity() = default;

    virtual void Update(float dt) = 0;
    virtual void Draw() const = 0;

    virtual void TakeDamage(float amount, Vector2 knockback, bool isCrit = false) {
        (void)isCrit;
        if (m_invulnTimer > 0.0f || m_isDead) return;

        m_hp -= amount;
        m_vel.x = knockback.x;
        m_vel.y = knockback.y;
        if (knockback.y < 0.0f) {
            m_isGrounded = false;
        }
        m_hurtTimer = 0.25f;
        m_flashTimer = 0.20f;
        m_invulnTimer = 0.15f;

        if (m_hp <= 0.0f) {
            m_hp = 0.0f;
            m_isDead = true;
        }
    }

    Rectangle GetBounds() const {
        return { m_pos.x - m_width * 0.5f, m_pos.y - m_height, m_width, m_height };
    }

    Hurtbox GetHurtbox() const {
        return { GetBounds(), m_faction, m_id };
    }

    // Accessors & Mutators
    int GetId() const { return m_id; }
    Faction GetFaction() const { return m_faction; }
    Vector2 GetPosition() const { return m_pos; }
    void SetPosition(Vector2 pos) { m_pos = pos; }
    Vector2 GetVelocity() const { return m_vel; }
    void SetVelocity(Vector2 vel) { m_vel = vel; }
    float GetFacing() const { return m_facing; }
    void SetFacing(float facing) { m_facing = facing; }
    float GetHP() const { return m_hp; }
    float GetMaxHP() const { return m_maxHp; }
    bool IsDead() const { return m_isDead; }
    bool IsGrounded() const { return m_isGrounded; }
    bool IsHurt() const { return m_hurtTimer > 0.0f; }
    bool IsFlashing() const { return m_flashTimer > 0.0f; }

protected:
    int m_id;
    Faction m_faction;
    Vector2 m_pos;   // Base center position (ground contact)
    Vector2 m_vel;
    float m_width;
    float m_height;
    float m_hp;
    float m_maxHp;
    float m_facing;
    bool m_isGrounded;
    bool m_isDead;
    float m_hurtTimer;
    float m_flashTimer;
    float m_invulnTimer;
};
