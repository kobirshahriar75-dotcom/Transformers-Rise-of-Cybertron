#pragma once

#include "Settings.hpp"
#include <cstdint>
#include <array>
#include <string>

struct PlayerUpgrades {
    int bladeDamageLevel = 0;       // Max 5
    int cannonDamageLevel = 0;      // Max 5
    int energonCapacityLevel = 0;   // Max 5
    int energonRegenLevel = 0;      // Max 5
    int transformationSpeedLevel = 0; // Max 5
    int truckAccelLevel = 0;        // Max 5
    int armorPlatingLevel = 0;      // Max 5
};

struct SaveData {
    uint32_t magic = 0x52425943;   // "CYBR"
    uint32_t version = 1;

    // Campaign Progression
    int highestUnlockedLevel = 1;   // 1 to 10
    std::array<bool, 10> completedLevels = { false };
    std::array<int, 10> levelHighScores = { 0 };
    int totalEnergonCores = 0;

    // Upgrades
    PlayerUpgrades upgrades;

    // Settings
    GameSettings settings;
};

class SaveSystem {
public:
    static const std::string SAVE_FILE_PATH;

    static bool Load(SaveData& outData);
    static bool Save(const SaveData& data);
    static void ResetToDefault(SaveData& outData);

    // Progression verification (Mandatory sequential unlock)
    static bool IsLevelUnlocked(const SaveData& data, int levelNumber); // levelNumber: 1 to 10
    static void RegisterLevelCompletion(SaveData& data, int levelNumber, int score);
    static bool CanReplayLevel(const SaveData& data, int levelNumber);

private:
    static uint32_t CalculateChecksum(const SaveData& data);
};
