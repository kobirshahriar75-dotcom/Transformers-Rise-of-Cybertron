#pragma once

#include "raylib.h"
#include "Constants.hpp"

class CameraManager {
public:
    CameraManager();

    void Init(Vector2 targetPos);
    void Update(Vector2 targetPos, float dt);
    void AddTrauma(float amount);

    Camera2D GetCamera() const { return m_camera; }
    Vector2 GetScreenShakeOffset() const { return m_shakeOffset; }
    void Reset(Vector2 targetPos);

private:
    Camera2D m_camera;
    Vector2 m_shakeOffset;
    float m_trauma; // 0.0 to 1.0
    float m_maxShakeX;
    float m_maxShakeY;
};
