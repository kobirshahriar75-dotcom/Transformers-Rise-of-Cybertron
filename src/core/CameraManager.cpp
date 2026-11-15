#include "CameraManager.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>

CameraManager::CameraManager()
    : m_shakeOffset{0.0f, 0.0f}
    , m_trauma(0.0f)
    , m_maxShakeX(22.0f)
    , m_maxShakeY(16.0f)
{
    m_camera.offset = { (float)Constants::SCREEN_WIDTH * 0.35f, (float)Constants::SCREEN_HEIGHT * 0.58f };
    m_camera.target = { 300.0f, Constants::GROUND_Y - 40.0f };
    m_camera.rotation = 0.0f;
    m_camera.zoom = 1.0f;
}

void CameraManager::Init(Vector2 targetPos) {
    m_camera.target = targetPos;
    m_trauma = 0.0f;
    m_shakeOffset = { 0.0f, 0.0f };
}

void CameraManager::Reset(Vector2 targetPos) {
    Init(targetPos);
}

void CameraManager::AddTrauma(float amount) {
    m_trauma = std::min(1.0f, m_trauma + amount);
}

void CameraManager::Update(Vector2 targetPos, float dt) {
    // Smooth camera following on X, gentle damp on Y
    float followSpeedX = 6.5f;
    float followSpeedY = 4.0f;

    m_camera.target.x += (targetPos.x - m_camera.target.x) * (followSpeedX * dt);
    m_camera.target.y += ((Constants::GROUND_Y - 80.0f) - m_camera.target.y) * (followSpeedY * dt);

    // Clamp camera within arena bounds
    float halfVisibleWidth = (float)Constants::SCREEN_WIDTH * 0.35f;
    float maxVisibleWidth = Constants::ARENA_WIDTH - ((float)Constants::SCREEN_WIDTH * 0.65f);

    if (m_camera.target.x < halfVisibleWidth) {
        m_camera.target.x = halfVisibleWidth;
    } else if (m_camera.target.x > maxVisibleWidth) {
        m_camera.target.x = maxVisibleWidth;
    }

    // Process Trauma & Screen Shake
    if (m_trauma > 0.0f) {
        // Non-linear trauma response (trauma^2) feels more visceral
        float shakeFactor = m_trauma * m_trauma;
        float randX = ((float)(rand() % 2000) / 1000.0f - 1.0f);
        float randY = ((float)(rand() % 2000) / 1000.0f - 1.0f);

        m_shakeOffset.x = randX * m_maxShakeX * shakeFactor;
        m_shakeOffset.y = randY * m_maxShakeY * shakeFactor;

        m_camera.offset.x = ((float)Constants::SCREEN_WIDTH * 0.35f) + m_shakeOffset.x;
        m_camera.offset.y = ((float)Constants::SCREEN_HEIGHT * 0.58f) + m_shakeOffset.y;

        // Decay trauma
        m_trauma = std::max(0.0f, m_trauma - 1.8f * dt);
    } else {
        m_shakeOffset = { 0.0f, 0.0f };
        m_camera.offset = { (float)Constants::SCREEN_WIDTH * 0.35f, (float)Constants::SCREEN_HEIGHT * 0.58f };
    }
}
