#include "SaveSystem.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <algorithm>

const std::string SaveSystem::SAVE_FILE_PATH = "save_data.dat";

namespace {
    constexpr uint32_t EXPECTED_MAGIC = 0x52425943; // "CYBR"
    constexpr uint32_t CURRENT_VERSION = 2;
}

uint32_t SaveSystem::CalculateChecksum(const SaveData& data) {
    // Hash individual fields rather than the raw struct representation.
    // Raw-struct hashing includes compiler padding bytes, which are not stable.
    uint32_t hash = 2166136261u;
    auto add = [&hash](const auto& value) {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
        for (size_t i = 0; i < sizeof(value); ++i) {
            hash ^= bytes[i];
            hash *= 16777619u;
        }
    };

    add(data.magic);
    add(data.version);
    add(data.highestUnlockedLevel);
    for (bool value : data.completedLevels) add(value);
    for (int value : data.levelHighScores) add(value);
    add(data.totalEnergonCores);

    add(data.upgrades.bladeDamageLevel);
    add(data.upgrades.cannonDamageLevel);
    add(data.upgrades.energonCapacityLevel);
    add(data.upgrades.energonRegenLevel);
    add(data.upgrades.transformationSpeedLevel);
    add(data.upgrades.truckAccelLevel);
    add(data.upgrades.armorPlatingLevel);

    const GameSettings& s = data.settings;
    add(s.musicEnabled); add(s.sfxEnabled);
    add(s.masterVolume); add(s.musicVolume); add(s.sfxVolume);
    add(s.colorPalette); add(s.brightness); add(s.contrast);
    add(s.screenWidth); add(s.screenHeight); add(s.fullscreen); add(s.vsync);
    add(s.screenShakeIntensity); add(s.visualEffectIntensity);
    add(s.subtitlesEnabled); add(s.difficulty); add(s.damageNumbers);
    add(s.cameraTrackingSpeed);
    return hash;
}

void SaveSystem::ResetToDefault(SaveData& outData) {
    outData = SaveData();
    outData.magic = EXPECTED_MAGIC;
    outData.version = CURRENT_VERSION;
    outData.highestUnlockedLevel = 1;
    outData.completedLevels.fill(false);
    outData.levelHighScores.fill(0);
    outData.totalEnergonCores = 0;
    outData.upgrades = PlayerUpgrades();
    outData.settings = GameSettings();
}

bool SaveSystem::Load(SaveData& outData) {
    std::ifstream file(SAVE_FILE_PATH, std::ios::binary);
    if (!file.is_open()) {
        // File does not exist yet; initialize default state and persist
        ResetToDefault(outData);
        Save(outData);
        return true;
    }

    SaveData temp;
    file.read(reinterpret_cast<char*>(&temp), sizeof(SaveData));
    if (!file.good()) {
        std::cerr << "[SaveSystem] Corrupted or incomplete save file. Resetting to default.\n";
        ResetToDefault(outData);
        Save(outData);
        return false;
    }

    uint32_t storedChecksum = 0;
    file.read(reinterpret_cast<char*>(&storedChecksum), sizeof(storedChecksum));
    if (!file || file.peek() != std::char_traits<char>::eof()) {
        std::cerr << "[SaveSystem] Save file has an invalid length. Resetting to default.\\n";
        ResetToDefault(outData);
        Save(outData);
        return false;
    }

    // Validate header and integrity
    if (temp.magic != EXPECTED_MAGIC || temp.version != CURRENT_VERSION) {
        std::cerr << "[SaveSystem] Invalid save magic/version. Resetting to default.\n";
        ResetToDefault(outData);
        Save(outData);
        return false;
    }

    uint32_t computedChecksum = CalculateChecksum(temp);
    if (storedChecksum != computedChecksum) {
        std::cerr << "[SaveSystem] Save checksum mismatch! Resetting to default.\n";
        ResetToDefault(outData);
        Save(outData);
        return false;
    }

    // Validate and normalize persisted values before exposing them to gameplay/UI.
    temp.highestUnlockedLevel = std::clamp(temp.highestUnlockedLevel, 1, 10);
    temp.totalEnergonCores = std::max(0, temp.totalEnergonCores);
    for (int& score : temp.levelHighScores) score = std::max(0, score);
    auto clampUpgrade = [](int& value) { value = std::clamp(value, 0, 5); };
    clampUpgrade(temp.upgrades.bladeDamageLevel);
    clampUpgrade(temp.upgrades.cannonDamageLevel);
    clampUpgrade(temp.upgrades.energonCapacityLevel);
    clampUpgrade(temp.upgrades.energonRegenLevel);
    clampUpgrade(temp.upgrades.transformationSpeedLevel);
    clampUpgrade(temp.upgrades.truckAccelLevel);
    clampUpgrade(temp.upgrades.armorPlatingLevel);
    temp.settings.masterVolume = std::clamp(temp.settings.masterVolume, 0.0f, 1.0f);
    temp.settings.musicVolume = std::clamp(temp.settings.musicVolume, 0.0f, 1.0f);
    temp.settings.sfxVolume = std::clamp(temp.settings.sfxVolume, 0.0f, 1.0f);
    temp.settings.colorPalette = std::clamp(temp.settings.colorPalette, 0, 2);
    temp.settings.brightness = std::clamp(temp.settings.brightness, 0.8f, 1.2f);
    temp.settings.contrast = std::clamp(temp.settings.contrast, 0.8f, 1.2f);
    temp.settings.screenShakeIntensity = std::clamp(temp.settings.screenShakeIntensity, 0.0f, 1.5f);
    temp.settings.visualEffectIntensity = std::clamp(temp.settings.visualEffectIntensity, 0.2f, 1.5f);
    temp.settings.difficulty = std::clamp(temp.settings.difficulty, 0, 2);
    temp.settings.cameraTrackingSpeed = std::clamp(temp.settings.cameraTrackingSpeed, 0.5f, 1.5f);
    temp.settings.screenWidth = std::clamp(temp.settings.screenWidth, 640, 7680);
    temp.settings.screenHeight = std::clamp(temp.settings.screenHeight, 480, 4320);
    outData = temp;
    return true;
}

bool SaveSystem::Save(const SaveData& data) {
    std::ofstream file(SAVE_FILE_PATH, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "[SaveSystem] Failed to open save file for writing.\n";
        return false;
    }

    file.write(reinterpret_cast<const char*>(&data), sizeof(SaveData));
    uint32_t checksum = CalculateChecksum(data);
    file.write(reinterpret_cast<const char*>(&checksum), sizeof(checksum));

    return file.good();
}

bool SaveSystem::IsLevelUnlocked(const SaveData& data, int levelNumber) {
    if (levelNumber <= 1) return true;
    if (levelNumber > 10) return false;

    // Strict sequential unlock rule:
    // Level N is unlocked if highestUnlockedLevel >= N, which occurs when Level N-1 is completed
    return data.highestUnlockedLevel >= levelNumber;
}

bool SaveSystem::CanReplayLevel(const SaveData& data, int levelNumber) {
    return IsLevelUnlocked(data, levelNumber);
}

void SaveSystem::RegisterLevelCompletion(SaveData& data, int levelNumber, int score) {
    if (levelNumber < 1 || levelNumber > 10) return;

    int idx = levelNumber - 1;
    const bool firstCompletion = !data.completedLevels[idx];
    data.completedLevels[idx] = true;
    if (score > data.levelHighScores[idx]) {
        data.levelHighScores[idx] = score;
    }

    // Unlock next level sequentially if not yet unlocked
    if (levelNumber < 10) {
        if (data.highestUnlockedLevel < levelNumber + 1) {
            data.highestUnlockedLevel = levelNumber + 1;
        }
    }

    // Award completion currency once per level; replaying cannot farm unlimited cores.
    if (firstCompletion) {
        data.totalEnergonCores += 100 + (std::max(0, score) / 10);
    }

    Save(data);
}
