#include "ParallaxBackground.hpp"
#include "../core/Constants.hpp"
#include <cmath>

ParallaxBackground::ParallaxBackground()
    : m_animTime(0.0f)
    , m_skyTop{ 10, 8, 24, 255 }
    , m_skyBottom{ 22, 28, 48, 255 }
    , m_neonColor{ 0, 210, 240, 255 }
{
}

ParallaxBackground::~ParallaxBackground() {
}

void ParallaxBackground::Init() {
    m_animTime = 0.0f;
    m_skyTop = { 10, 8, 24, 255 };
    m_skyBottom = { 22, 28, 48, 255 };
    m_neonColor = { 0, 210, 240, 255 };
}

void ParallaxBackground::SetTheme(Color skyTop, Color skyBottom, Color neonColor) {
    m_skyTop = skyTop;
    m_skyBottom = skyBottom;
    m_neonColor = neonColor;
}

void ParallaxBackground::Update(float dt) {
    m_animTime += dt;
}

void ParallaxBackground::DrawSky(const Camera2D& camera) const {
    DrawRectangleGradientV(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, m_skyTop, m_skyBottom);

    // Distant Cybertronian metallic moon / Primus sphere
    float moonX = 980.0f - (camera.target.x * 0.02f);
    float moonY = 160.0f;
    DrawCircle((int)moonX, (int)moonY, 70.0f, { 60, 75, 110, 80 });
    DrawCircle((int)moonX, (int)moonY, 60.0f, { 140, 160, 210, 120 });
    DrawCircleLines((int)moonX, (int)moonY, 60.0f, { 200, 220, 255, 180 });
    DrawLine((int)moonX - 50, (int)moonY, (int)moonX + 50, (int)moonY, { 180, 200, 240, 120 });
    DrawCircleLines((int)moonX, (int)moonY, 40.0f, { 180, 200, 240, 100 });
}

void ParallaxBackground::DrawDistantSpires(const Camera2D& camera) const {
    float offsetX = -camera.target.x * 0.10f;
    Color spireColor = { 26, 32, 54, 255 };

    float baseSpacing = 160.0f;
    int spireCount = (int)(Constants::ARENA_WIDTH / baseSpacing) + 12;

    for (int i = -4; i < spireCount; i++) {
        float x = (i * baseSpacing) + offsetX + 200.0f;
        if (x < -200.0f || x > Constants::SCREEN_WIDTH + 200.0f) continue;

        float height = 240.0f + ((i * 47) % 190);
        float width = 50.0f + ((i * 19) % 35);
        float y = Constants::GROUND_Y - height + 40.0f;

        DrawRectangle((int)x, (int)y, (int)width, (int)height, spireColor);
        DrawTriangle(
            { x, y },
            { x + width * 0.5f, y - 45.0f },
            { x + width, y },
            spireColor
        );

        float pulse = (sinf(m_animTime * 2.0f + (float)i) + 1.0f) * 0.5f;
        Color neon = m_neonColor;
        neon.a = (unsigned char)(60 + pulse * 140);
        for (int w = 0; w < 4; w++) {
            float wy = y + 30.0f + (w * 35.0f);
            DrawRectangle((int)x + 6, (int)wy, (int)width - 12, 4, neon);
        }
    }
}

void ParallaxBackground::DrawMidStructures(const Camera2D& camera) const {
    float offsetX = -camera.target.x * 0.35f;
    Color factoryColor = { 34, 42, 68, 255 };

    float baseSpacing = 320.0f;
    int count = (int)(Constants::ARENA_WIDTH / baseSpacing) + 6;

    for (int i = -2; i < count; i++) {
        float x = (i * baseSpacing) + offsetX;
        if (x < -350.0f || x > Constants::SCREEN_WIDTH + 350.0f) continue;

        float h = 180.0f + ((i * 61) % 120);
        float w = 180.0f;
        float y = Constants::GROUND_Y - h + 20.0f;

        DrawRectangle((int)x, (int)y, (int)w, (int)h, factoryColor);
        DrawRectangleLines((int)x, (int)y, (int)w, (int)h, { 55, 68, 105, 255 });

        float pulse = (sinf(m_animTime * 4.0f + (float)i * 1.5f) + 1.0f) * 0.5f;
        Color purpleGlow = Constants::ENERGON_PURPLE;
        purpleGlow.a = (unsigned char)(100 + pulse * 130);
        DrawRectangle((int)x + 20, (int)y + 40, (int)w - 40, 8, purpleGlow);
    }
}

void ParallaxBackground::DrawNearPylons(const Camera2D& camera) const {
    float offsetX = -camera.target.x * 0.65f;
    Color pylonDark = { 42, 48, 72, 255 };

    float baseSpacing = 520.0f;
    int count = (int)(Constants::ARENA_WIDTH / baseSpacing) + 4;

    for (int i = -1; i < count; i++) {
        float x = (i * baseSpacing) + offsetX;
        if (x < -150.0f || x > Constants::SCREEN_WIDTH + 150.0f) continue;

        float w = 48.0f;
        float y = Constants::GROUND_Y - 260.0f;
        float h = 280.0f;

        DrawRectangle((int)x, (int)y, (int)w, (int)h, pylonDark);
        DrawRectangleLines((int)x, (int)y, (int)w, (int)h, { 70, 82, 120, 255 });

        for (int c = 0; c < 5; c++) {
            float cy = y + 25.0f + (c * 45.0f);
            DrawTriangle(
                { x + 4.0f, cy },
                { x + w - 4.0f, cy },
                { x + w * 0.5f, cy + 20.0f },
                { 255, 180, 20, 160 }
            );
        }
    }
}

void ParallaxBackground::Draw(const Camera2D& camera) const {
    DrawSky(camera);
    DrawDistantSpires(camera);
    DrawMidStructures(camera);
    DrawNearPylons(camera);
}

void ParallaxBackground::DrawGroundPlatform(const Camera2D& camera) const {
    (void)camera;
    float startX = 0.0f;
    float endX = Constants::ARENA_WIDTH;
    float groundY = Constants::GROUND_Y;
    float groundH = 300.0f;

    Color plateDark = { 28, 34, 48, 255 };
    DrawRectangle((int)startX, (int)groundY, (int)(endX - startX), (int)groundH, plateDark);

    Color neonTrim = m_neonColor;
    DrawRectangle((int)startX, (int)groundY, (int)(endX - startX), 6, neonTrim);
    DrawRectangle((int)startX, (int)groundY + 6, (int)(endX - startX), 2, RAYWHITE);

    float secWidth = 200.0f;
    for (float x = startX; x < endX; x += secWidth) {
        DrawLineEx({ x, groundY + 6.0f }, { x, groundY + groundH }, 2.0f, { 18, 22, 34, 255 });
        DrawCircle((int)x + 10, (int)groundY + 16, 3.0f, { 80, 95, 130, 255 });
        DrawCircle((int)x + (int)secWidth - 10, (int)groundY + 16, 3.0f, { 80, 95, 130, 255 });
    }
}
