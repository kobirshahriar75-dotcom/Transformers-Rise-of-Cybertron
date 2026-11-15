#pragma once

#include "raylib.h"

class ParallaxBackground {
public:
    ParallaxBackground();
    ~ParallaxBackground();

    void Init();
    void SetTheme(Color skyTop, Color skyBottom, Color neonColor);
    void Update(float dt);
    void Draw(const Camera2D& camera) const;
    void DrawGroundPlatform(const Camera2D& camera) const;

private:
    float m_animTime;
    Color m_skyTop;
    Color m_skyBottom;
    Color m_neonColor;

    void DrawSky(const Camera2D& camera) const;
    void DrawDistantSpires(const Camera2D& camera) const;
    void DrawMidStructures(const Camera2D& camera) const;
    void DrawNearPylons(const Camera2D& camera) const;
};
