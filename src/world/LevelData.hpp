#pragma once

#include "raylib.h"
#include <string>
#include <vector>

enum class LevelBossType {
    NONE,
    SHOCKWAVE,
    SOUNDWAVE,
    STARSCREAM,
    MEGATRON
};

struct LevelInfo {
    int number;                 // 1 to 10
    std::string name;           // e.g. "Abandoned City"
    std::string subtitle;       // e.g. "Sector 7 - Perimeter"
    std::string description;    // Mission briefing
    LevelBossType bossType;     // Boss encounter if applicable
    int totalWaves;             // Standard wave count
    Color themeSkyTop;          // Sky gradient top
    Color themeSkyBottom;       // Sky gradient bottom
    Color groundColor;          // Floor platform tint
    Color neonColor;            // Energy conduit glow color
};

namespace LevelDatabase {
    inline const std::vector<LevelInfo> CAMPAIGN_LEVELS = {
        {
            1,
            "Abandoned City",
            "Sector 12 - Urban Ruin",
            "Scout patrols detected in the ruined metropolitan zone. Clear the forward recon squads.",
            LevelBossType::NONE,
            3,
            { 28, 18, 48, 255 }, { 55, 30, 80, 255 },
            { 40, 42, 54, 255 }, { 0, 220, 255, 255 }
        },
        {
            2,
            "Industrial District",
            "Foundry Sector - Smelting Complex",
            "Heavy Decepticon enforcers are extracting Energon reserves. Neutralize the heavy guard.",
            LevelBossType::NONE,
            3,
            { 55, 22, 10, 255 }, { 95, 45, 18, 255 },
            { 50, 44, 40, 255 }, { 255, 140, 20, 255 }
        },
        {
            3,
            "Desert Battlefield",
            "The Rust Sea - Iron Dunes",
            "Cross the iron wastes under heavy Decepticon perimeter fire. Engage high-speed scouts.",
            LevelBossType::NONE,
            4,
            { 45, 32, 20, 255 }, { 80, 58, 32, 255 },
            { 60, 52, 42, 255 }, { 240, 190, 40, 255 }
        },
        {
            4,
            "Cybertron Ruins",
            "Ancient Temple Grounds - Shockwave Encampment",
            "Shockwave has secured ancient Cybertronian glyph tech. Terminate the head scientist.",
            LevelBossType::SHOCKWAVE,
            3,
            { 18, 12, 38, 255 }, { 42, 20, 75, 255 },
            { 36, 38, 52, 255 }, { 180, 40, 240, 255 }
        },
        {
            5,
            "Frozen Facility",
            "Sub-Zero Outpost - Cryo Vaults",
            "Navigate cryogenic storage chambers infested with frost-shielded Decepticon units.",
            LevelBossType::NONE,
            3,
            { 15, 35, 55, 255 }, { 30, 70, 95, 255 },
            { 45, 60, 75, 255 }, { 140, 230, 255, 255 }
        },
        {
            6,
            "Decepticon Base",
            "Communications Fortress - Soundwave Lair",
            "Infiltrate the broadcast citadel. Soundwave controls orbital transmission arrays.",
            LevelBossType::SOUNDWAVE,
            3,
            { 22, 14, 44, 255 }, { 50, 25, 88, 255 },
            { 38, 35, 50, 255 }, { 220, 30, 240, 255 }
        },
        {
            7,
            "Aerial Battlefield",
            "Stratosphere - Anti-Gravity Platforms",
            "Dogfight over cloud platforms high above Cybertron's iron canopy.",
            LevelBossType::NONE,
            4,
            { 20, 30, 65, 255 }, { 45, 65, 115, 255 },
            { 48, 55, 75, 255 }, { 0, 235, 255, 255 }
        },
        {
            8,
            "Destroyed Metropolis",
            "Capital Plaza - Starscream Air Strike",
            "Starscream commands seeker squadrons bombing civilian refuge zones. Bring him down.",
            LevelBossType::STARSCREAM,
            3,
            { 60, 20, 20, 255 }, { 110, 40, 30, 255 },
            { 52, 42, 42, 255 }, { 255, 60, 40, 255 }
        },
        {
            9,
            "Cybertron Outskirts",
            "The Great Trench - Elite Decepticon Gauntlet",
            "Break through Megatron's personal guard vanguard. High concentration of armored elites.",
            LevelBossType::NONE,
            4,
            { 35, 10, 35, 255 }, { 70, 22, 70, 255 },
            { 45, 38, 48, 255 }, { 210, 40, 230, 255 }
        },
        {
            10,
            "Final Battlefield",
            "The Well of AllSparks - Megatron Showdown",
            "The fate of Cybertron hangs in the balance. Defeat Megatron in single combat.",
            LevelBossType::MEGATRON,
            4,
            { 12, 8, 22, 255 }, { 32, 16, 50, 255 },
            { 30, 28, 40, 255 }, { 255, 30, 50, 255 }
        }
    };

    inline const LevelInfo& GetLevel(int levelNumber) {
        if (levelNumber < 1) levelNumber = 1;
        if (levelNumber > 10) levelNumber = 10;
        return CAMPAIGN_LEVELS[levelNumber - 1];
    }
}
