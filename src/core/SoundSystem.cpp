#include "SoundSystem.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <vector>

namespace {
    constexpr float PI_F = 3.14159265358979323846f;
    constexpr float MENU_MUSIC_GAIN = 0.90f;
    constexpr float GAMEPLAY_MUSIC_GAIN = 0.70f;

    inline float ClampF(float v, float minV, float maxV) {
        if (v < minV) return minV;
        if (v > maxV) return maxV;
        return v;
    }

    inline float RandFloat() {
        return (float)rand() / (float)RAND_MAX;
    }

    // Resolve assets from either the project working directory or the executable
    // directory. This keeps music available when launched from VS Code, make run,
    // or by opening the executable inside bin/ directly.
    std::string ResolveMusicAssetPath(const std::string& relativePath) {
        const std::string appDir = GetApplicationDirectory();
        const std::vector<std::string> candidates = {
            relativePath,
            appDir + relativePath,
            appDir + "../" + relativePath,
            appDir + "../../" + relativePath
        };
        for (const std::string& candidate : candidates) {
            if (FileExists(candidate.c_str())) return candidate;
        }
        return relativePath;
    }

    // Procedural SFX generators
    void GenSlashLight(short* buffer, int count, int sampleRate) {
        for (int i = 0; i < count; i++) {
            float t = (float)i / (float)sampleRate;
            float dur = (float)count / (float)sampleRate;
            float progress = t / dur;
            float freq = 650.0f - (progress * 450.0f);
            float sine = sinf(2.0f * PI_F * freq * t);
            float noise = (RandFloat() * 2.0f - 1.0f);
            float env = (1.0f - progress) * (1.0f - progress);
            float sample = (sine * 0.4f + noise * 0.6f) * env;
            buffer[i] = (short)(ClampF(sample, -1.0f, 1.0f) * 24000.0f);
        }
    }

    void GenSlashHeavy(short* buffer, int count, int sampleRate) {
        for (int i = 0; i < count; i++) {
            float t = (float)i / (float)sampleRate;
            float dur = (float)count / (float)sampleRate;
            float progress = t / dur;
            float freq = 380.0f - (progress * 300.0f);
            float sine = sinf(2.0f * PI_F * freq * t);
            float noise = (RandFloat() * 2.0f - 1.0f);
            float env = (1.0f - progress);
            float sample = (sine * 0.6f + noise * 0.4f) * env;
            buffer[i] = (short)(ClampF(sample, -1.0f, 1.0f) * 28000.0f);
        }
    }

    void GenImpactMetal(short* buffer, int count, int sampleRate) {
        for (int i = 0; i < count; i++) {
            float t = (float)i / (float)sampleRate;
            float dur = (float)count / (float)sampleRate;
            float progress = t / dur;
            float s1 = sinf(2.0f * PI_F * 520.0f * t);
            float s2 = sinf(2.0f * PI_F * 840.0f * t);
            float s3 = sinf(2.0f * PI_F * 1230.0f * t);
            float s4 = sinf(2.0f * PI_F * 2150.0f * t);
            float noise = (RandFloat() * 2.0f - 1.0f) * 0.35f;
            float env = expf(-progress * 12.0f);
            float sample = (s1 * 0.3f + s2 * 0.25f + s3 * 0.2f + s4 * 0.15f + noise) * env;
            buffer[i] = (short)(ClampF(sample, -1.0f, 1.0f) * 29000.0f);
        }
    }

    void GenBlaster(short* buffer, int count, int sampleRate) {
        for (int i = 0; i < count; i++) {
            float t = (float)i / (float)sampleRate;
            float dur = (float)count / (float)sampleRate;
            float progress = t / dur;
            float freq = 1200.0f * expf(-progress * 9.0f);
            float sine = sinf(2.0f * PI_F * freq * t);
            float sq = (sine > 0.0f) ? 1.0f : -1.0f;
            float env = (1.0f - progress);
            float sample = (sine * 0.6f + sq * 0.4f) * env;
            buffer[i] = (short)(ClampF(sample, -1.0f, 1.0f) * 26000.0f);
        }
    }

    void GenExplosion(short* buffer, int count, int sampleRate) {
        for (int i = 0; i < count; i++) {
            float t = (float)i / (float)sampleRate;
            float dur = (float)count / (float)sampleRate;
            float progress = t / dur;
            float rumbleFreq = 65.0f * (1.0f - progress * 0.5f);
            float rumble = sinf(2.0f * PI_F * rumbleFreq * t);
            float noise = (RandFloat() * 2.0f - 1.0f);
            float env = expf(-progress * 5.5f);
            float sample = (rumble * 0.4f + noise * 0.7f) * env;
            buffer[i] = (short)(ClampF(sample, -1.0f, 1.0f) * 30000.0f);
        }
    }

    void GenTransform(short* buffer, int count, int sampleRate) {
        float notes[5] = { 220.0f, 330.0f, 440.0f, 550.0f, 660.0f };
        int subCount = count / 5;
        for (int i = 0; i < count; i++) {
            int noteIdx = std::min(4, i / subCount);
            int localI = i % subCount;
            float t = (float)localI / (float)sampleRate;
            float progress = (float)localI / (float)subCount;
            float freq = notes[noteIdx];
            float s = sinf(2.0f * PI_F * freq * t);
            float metallic = sinf(2.0f * PI_F * freq * 2.5f * t) * 0.25f;
            float env = sinf(progress * PI_F);
            float sample = (s + metallic) * env;
            buffer[i] = (short)(ClampF(sample, -1.0f, 1.0f) * 24000.0f);
        }
    }

    void GenTruckHorn(short* buffer, int count, int sampleRate) {
        for (int i = 0; i < count; i++) {
            float t = (float)i / (float)sampleRate;
            float dur = (float)count / (float)sampleRate;
            float progress = t / dur;
            float f1 = 233.08f;
            float f2 = 293.66f;
            float s1 = sinf(2.0f * PI_F * f1 * t);
            float s2 = sinf(2.0f * PI_F * f2 * t);
            float env = (progress < 0.1f) ? (progress / 0.1f) : (1.0f - (progress - 0.1f) / 0.9f);
            float sample = (s1 * 0.5f + s2 * 0.5f) * env;
            buffer[i] = (short)(ClampF(sample, -1.0f, 1.0f) * 28000.0f);
        }
    }

    void GenTruckRam(short* buffer, int count, int sampleRate) {
        for (int i = 0; i < count; i++) {
            float t = (float)i / (float)sampleRate;
            float dur = (float)count / (float)sampleRate;
            float progress = t / dur;
            float f = 110.0f * (1.0f - progress * 0.8f);
            float s = sinf(2.0f * PI_F * f * t);
            float noise = (RandFloat() * 2.0f - 1.0f) * 0.5f;
            float env = expf(-progress * 8.0f);
            float sample = (s + noise) * env;
            buffer[i] = (short)(ClampF(sample, -1.0f, 1.0f) * 29000.0f);
        }
    }

    void GenHurt(short* buffer, int count, int sampleRate) {
        for (int i = 0; i < count; i++) {
            float t = (float)i / (float)sampleRate;
            float dur = (float)count / (float)sampleRate;
            float progress = t / dur;
            float f = 200.0f - progress * 140.0f;
            float s = sinf(2.0f * PI_F * f * t);
            float env = 1.0f - progress;
            float sample = s * env;
            buffer[i] = (short)(ClampF(sample, -1.0f, 1.0f) * 22000.0f);
        }
    }

    void GenBossRoar(short* buffer, int count, int sampleRate) {
        for (int i = 0; i < count; i++) {
            float t = (float)i / (float)sampleRate;
            float dur = (float)count / (float)sampleRate;
            float progress = t / dur;
            float f = 85.0f + sinf(t * 18.0f) * 35.0f;
            float s = sinf(2.0f * PI_F * f * t);
            float noise = (RandFloat() * 2.0f - 1.0f) * 0.6f;
            float env = sinf(progress * PI_F);
            float sample = (s * 0.6f + noise * 0.5f) * env;
            buffer[i] = (short)(ClampF(sample, -1.0f, 1.0f) * 30000.0f);
        }
    }

    void GenBossBeam(short* buffer, int count, int sampleRate) {
        for (int i = 0; i < count; i++) {
            float t = (float)i / (float)sampleRate;
            float dur = (float)count / (float)sampleRate;
            float progress = t / dur;
            float f = 440.0f + sinf(t * 30.0f) * 60.0f;
            float s = sinf(2.0f * PI_F * f * t);
            float env = (1.0f - progress * 0.5f);
            float sample = s * env;
            buffer[i] = (short)(ClampF(sample, -1.0f, 1.0f) * 25000.0f);
        }
    }

    void GenSonicBlast(short* buffer, int count, int sampleRate) {
        for (int i = 0; i < count; i++) {
            float t = (float)i / (float)sampleRate;
            float dur = (float)count / (float)sampleRate;
            float progress = t / dur;
            float f = 900.0f - progress * 650.0f;
            float s = sinf(2.0f * PI_F * f * t) * cosf(2.0f * PI_F * 45.0f * t);
            float env = expf(-progress * 6.0f);
            float sample = s * env;
            buffer[i] = (short)(ClampF(sample, -1.0f, 1.0f) * 26000.0f);
        }
    }

    void GenMissileFire(short* buffer, int count, int sampleRate) {
        for (int i = 0; i < count; i++) {
            float t = (float)i / (float)sampleRate;
            float dur = (float)count / (float)sampleRate;
            float progress = t / dur;
            float f = 320.0f + progress * 800.0f;
            float s = sinf(2.0f * PI_F * f * t);
            float noise = (RandFloat() * 2.0f - 1.0f) * 0.45f;
            float env = 1.0f - progress;
            float sample = (s * 0.5f + noise * 0.5f) * env;
            buffer[i] = (short)(ClampF(sample, -1.0f, 1.0f) * 27000.0f);
        }
    }
}

std::string SoundSystem::GetMenuMusicFilePath() {
    return "assets/music/menu/Optimus.ogg";
}

std::string SoundSystem::GetGameplayMusicFilePath() {
    return "assets/music/campaign/Optimus_Prime_vs._Megatron.ogg";
}

SoundSystem::SoundSystem()
    : m_externalMenuMusic{}
    , m_externalGameplayMusic{}
    , m_hasExternalMenu(false)
    , m_hasExternalGameplay(false)
    , m_currentTrack(MusicTrack::NONE)
    , m_isPaused(false)
    , m_audioReady(false)
    , m_bgmVolume(0.75f)
    , m_sfxVolume(0.85f)
{
}

SoundSystem::~SoundSystem() {
    Shutdown();
}

Sound SoundSystem::GenerateWaveSound(int sampleRate, float duration, void (*generator)(short*, int, int)) {
    int sampleCount = (int)(sampleRate * duration);
    Wave wave;
    wave.frameCount = (unsigned int)sampleCount;
    wave.sampleRate = (unsigned int)sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = malloc(sampleCount * sizeof(short));

    generator((short*)wave.data, sampleCount, sampleRate);
    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    return sound;
}

void SoundSystem::GenerateAllSounds() {
    int sr = 22050;
    m_sounds.push_back(GenerateWaveSound(sr, 0.18f, GenSlashLight));
    m_sounds.push_back(GenerateWaveSound(sr, 0.28f, GenSlashHeavy));
    m_sounds.push_back(GenerateWaveSound(sr, 0.22f, GenImpactMetal));
    m_sounds.push_back(GenerateWaveSound(sr, 0.20f, GenBlaster));
    m_sounds.push_back(GenerateWaveSound(sr, 0.55f, GenExplosion));
    m_sounds.push_back(GenerateWaveSound(sr, 0.45f, GenTransform));
    m_sounds.push_back(GenerateWaveSound(sr, 0.65f, GenTruckHorn));
    m_sounds.push_back(GenerateWaveSound(sr, 0.35f, GenTruckRam));
    m_sounds.push_back(GenerateWaveSound(sr, 0.16f, GenHurt));
    m_sounds.push_back(GenerateWaveSound(sr, 0.70f, GenBossRoar));
    m_sounds.push_back(GenerateWaveSound(sr, 0.45f, GenBossBeam));
    m_sounds.push_back(GenerateWaveSound(sr, 0.35f, GenSonicBlast));
    m_sounds.push_back(GenerateWaveSound(sr, 0.30f, GenMissileFire));
}

void SoundSystem::Init() {
    if (m_audioReady) return;

    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        std::cerr << "[SoundSystem] Warning: Audio device not ready.\n";
        return;
    }

    GenerateAllSounds();

    // These two external files are the only background music sources.
    const std::string menuPath = ResolveMusicAssetPath(GetMenuMusicFilePath());
    if (FileExists(menuPath.c_str())) {
        m_externalMenuMusic = LoadMusicStream(menuPath.c_str());
        if (IsMusicValid(m_externalMenuMusic)) {
            m_externalMenuMusic.looping = true;
            m_hasExternalMenu = true;
            std::cout << "[SoundSystem] Loaded menu score: " << menuPath << "\n";
        }
    }
    if (!m_hasExternalMenu) {
        std::cerr << "[SoundSystem] ERROR: Required menu score missing or invalid: " << menuPath << "\n";
    }

    const std::string gameplayPath = ResolveMusicAssetPath(GetGameplayMusicFilePath());
    if (FileExists(gameplayPath.c_str())) {
        m_externalGameplayMusic = LoadMusicStream(gameplayPath.c_str());
        if (IsMusicValid(m_externalGameplayMusic)) {
            m_externalGameplayMusic.looping = true;
            m_hasExternalGameplay = true;
            std::cout << "[SoundSystem] Loaded gameplay score: " << gameplayPath << "\n";
        }
    }
    if (!m_hasExternalGameplay) {
        std::cerr << "[SoundSystem] ERROR: Required gameplay score missing or invalid: " << gameplayPath << "\n";
    }

    m_audioReady = true;
    m_isPaused = false;
    std::cout << "[SoundSystem] Audio device ready. Music master gain: " << m_bgmVolume << "\n";
    PlayMusicTrack(MusicTrack::MENU);
}

void SoundSystem::PlayMusicTrack(MusicTrack track) {
    if (!m_audioReady) return;

    // Do not early-return merely because the requested track matches: a stream
    // may have stopped or failed to start. Verify actual playback first.
    if (m_currentTrack == track && !m_isPaused) {
        const bool requestedTrackPlaying =
            (track == MusicTrack::MENU && m_hasExternalMenu && IsMusicStreamPlaying(m_externalMenuMusic)) ||
            (track == MusicTrack::GAMEPLAY && m_hasExternalGameplay && IsMusicStreamPlaying(m_externalGameplayMusic)) ||
            (track == MusicTrack::NONE);
        if (requestedTrackPlaying) return;
    }

    // Stop both streams unconditionally when switching, including a stream
    // that was paused (IsMusicStreamPlaying is false for paused streams).
    if (m_hasExternalMenu) StopMusicStream(m_externalMenuMusic);
    if (m_hasExternalGameplay) StopMusicStream(m_externalGameplayMusic);

    m_currentTrack = track;
    m_isPaused = false;

    if (track == MusicTrack::MENU && m_hasExternalMenu) {
        SetMusicVolume(m_externalMenuMusic, m_bgmVolume * MENU_MUSIC_GAIN);
        PlayMusicStream(m_externalMenuMusic);
        std::cout << "[SoundSystem] Playing menu score at effective volume "
                  << (m_bgmVolume * MENU_MUSIC_GAIN) << "\n";
    } else if (track == MusicTrack::GAMEPLAY && m_hasExternalGameplay) {
        SetMusicVolume(m_externalGameplayMusic, m_bgmVolume * GAMEPLAY_MUSIC_GAIN);
        PlayMusicStream(m_externalGameplayMusic);
        std::cout << "[SoundSystem] Playing gameplay score at effective volume "
                  << (m_bgmVolume * GAMEPLAY_MUSIC_GAIN) << "\n";
    } else if (track != MusicTrack::NONE) {
        std::cerr << "[SoundSystem] ERROR: Requested music track is unavailable.\n";
    }
    // MusicTrack::NONE intentionally leaves both external streams stopped.
}

void SoundSystem::PauseMusic() {
    m_isPaused = true;
    if (m_hasExternalMenu && IsMusicStreamPlaying(m_externalMenuMusic)) {
        PauseMusicStream(m_externalMenuMusic);
    }
    if (m_hasExternalGameplay && IsMusicStreamPlaying(m_externalGameplayMusic)) {
        PauseMusicStream(m_externalGameplayMusic);
    }
}

void SoundSystem::ResumeMusic() {
    m_isPaused = false;
    if (m_currentTrack == MusicTrack::MENU) {
        if (m_hasExternalMenu) {
            ResumeMusicStream(m_externalMenuMusic);
        }
    } else if (m_currentTrack == MusicTrack::GAMEPLAY) {
        if (m_hasExternalGameplay) {
            ResumeMusicStream(m_externalGameplayMusic);
        }
    }
}

void SoundSystem::Update(float dt) {
    (void)dt;
    if (!m_audioReady || m_isPaused) return;

    if (m_currentTrack == MusicTrack::MENU && m_hasExternalMenu) {
        UpdateMusicStream(m_externalMenuMusic);
    } else if (m_currentTrack == MusicTrack::GAMEPLAY && m_hasExternalGameplay) {
        UpdateMusicStream(m_externalGameplayMusic);
    }
}

void SoundSystem::Play(SoundType type, float pitch, float volume) {
    if (!m_audioReady || m_isPaused) return;
    int idx = (int)type;
    if (idx >= 0 && idx < (int)m_sounds.size()) {
        SetSoundPitch(m_sounds[idx], pitch);
        SetSoundVolume(m_sounds[idx], volume * m_sfxVolume);
        PlaySound(m_sounds[idx]);
    }
}

void SoundSystem::SetBgmVolume(float vol) {
    m_bgmVolume = std::clamp(vol, 0.0f, 1.0f);
    if (m_hasExternalMenu) SetMusicVolume(m_externalMenuMusic, m_bgmVolume * MENU_MUSIC_GAIN);
    if (m_hasExternalGameplay) SetMusicVolume(m_externalGameplayMusic, m_bgmVolume * GAMEPLAY_MUSIC_GAIN);
}

void SoundSystem::SetSfxVolume(float vol) {
    m_sfxVolume = std::clamp(vol, 0.0f, 1.0f);
}

void SoundSystem::StopAllSounds() {
    for (const auto& sound : m_sounds) {
        if (IsSoundValid(sound)) StopSound(sound);
    }
}

void SoundSystem::Shutdown() {
    if (!m_audioReady) return;

    if (m_hasExternalMenu) {
        UnloadMusicStream(m_externalMenuMusic);
        m_hasExternalMenu = false;
    }
    if (m_hasExternalGameplay) {
        UnloadMusicStream(m_externalGameplayMusic);
        m_hasExternalGameplay = false;
    }

    for (auto& s : m_sounds) {
        UnloadSound(s);
    }
    m_sounds.clear();

    CloseAudioDevice();
    m_audioReady = false;
}
