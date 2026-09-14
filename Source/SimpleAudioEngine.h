#pragma once

#include "axmol/audio/AudioEngine.h"

#include <string>
#include <unordered_map>

// Compatibility facade for the Cocos2d-x SimpleAudioEngine API used by the
// original game.  New code should call ax::AudioEngine directly.
namespace CocosDenshion
{
class SimpleAudioEngine
{
public:
    static SimpleAudioEngine* getInstance()
    {
        static SimpleAudioEngine instance;
        return &instance;
    }

    void preloadEffect(const char* file) { ax::AudioEngine::preload(file); }

    int playEffect(const char* file, bool loop = false, float pitch = 1.0f, float pan = 0.0f, float gain = 1.0f)
    {
        const auto id = ax::AudioEngine::play2d(file, loop, gain);
        ax::AudioEngine::setPitch(id, pitch);
        ax::AudioEngine::setPan(id, pan);
        return id;
    }

    void stopEffect(int id) { ax::AudioEngine::stop(id); }

    void playBackgroundMusic(const char* file, bool loop = false)
    {
        stopBackgroundMusic();
        _backgroundMusic = ax::AudioEngine::play2d(file, loop, _backgroundVolume);
    }

    void stopBackgroundMusic(bool = false)
    {
        if (_backgroundMusic != ax::AudioEngine::INVALID_AUDIO_ID)
            ax::AudioEngine::stop(_backgroundMusic);
        _backgroundMusic = ax::AudioEngine::INVALID_AUDIO_ID;
    }

    void pauseBackgroundMusic()
    {
        if (_backgroundMusic != ax::AudioEngine::INVALID_AUDIO_ID)
            ax::AudioEngine::pause(_backgroundMusic);
    }

    void resumeBackgroundMusic()
    {
        if (_backgroundMusic != ax::AudioEngine::INVALID_AUDIO_ID)
            ax::AudioEngine::resume(_backgroundMusic);
    }

    void setBackgroundMusicVolume(float volume)
    {
        _backgroundVolume = volume;
        if (_backgroundMusic != ax::AudioEngine::INVALID_AUDIO_ID)
            ax::AudioEngine::setVolume(_backgroundMusic, volume);
    }

    float getBackgroundMusicVolume() const { return _backgroundVolume; }
    void setEffectsVolume(float volume) { _effectsVolume = volume; }
    float getEffectsVolume() const { return _effectsVolume; }

private:
    ax::AudioId _backgroundMusic{ax::AudioEngine::INVALID_AUDIO_ID};
    float _backgroundVolume{1.0f};
    float _effectsVolume{1.0f};
};
}  // namespace CocosDenshion
