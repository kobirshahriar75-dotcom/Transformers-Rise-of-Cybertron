#pragma once

#include "raylib.h"
#include <vector>
#include <string>

enum class SoundType {
    SLASH_LIGHT,
    SLASH_HEAVY,
    IMPACT_METAL,
    BLASTER,
    EXPLOSION,
    TRANSFORM,
    TRUCK_HORN,
    TRUCK_RAM,
    HURT,
    BOSS_ROAR,
    BOSS_BEAM,
    SONIC_BLAST,
    MISSILE_FIRE
};

enum class MusicTrack {
    NONE,
    MENU,
    GAMEPLAY
};

class SoundSystem {
public:
    SoundSystem();
    ~SoundSystem();

    void Init();
    void Play(SoundType type, float pitch = 1.0f, float volume = 1.0f);
    void Update(float dt);
    void Shutdown();

    void PlayMusicTrack(MusicTrack track);
    void PauseMusic();
    void ResumeMusic();
    void StopAllSounds();
    MusicTrack GetCurrentTrack() const { return m_currentTrack; }

    void SetBgmVolume(float vol);
    void SetSfxVolume(float vol);

    // Required external music asset locations.
    static std::string GetMenuMusicFilePath();
    static std::string GetGameplayMusicFilePath();

private:
    void GenerateAllSounds();
    Sound GenerateWaveSound(int sampleRate, float duration, void (*generator)(short*, int, int));

    std::vector<Sound> m_sounds;
    Music m_externalMenuMusic;
    Music m_externalGameplayMusic;
    bool m_hasExternalMenu;
    bool m_hasExternalGameplay;

    MusicTrack m_currentTrack;
    bool m_isPaused;
    bool m_audioReady;
    float m_bgmVolume;
    float m_sfxVolume;
};
