#pragma once

#include "raylib.h"
#include "../core/Constants.hpp"

struct Hitbox {
    Rectangle rect;
    Faction faction;
    float damage;
    Vector2 knockback;
    bool isCrit;
    bool active;
    int attackId;
    float hitStopDuration;
    bool causesScreenShake;
};

struct Hurtbox {
    Rectangle rect;
    Faction faction;
    int entityId;
};
