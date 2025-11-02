#pragma once

#include "clipod/decoder.h"

#include <AL/al.h>

#include <modern-uuid/uuid.h>

#include <sigslot/signal.hpp>

#include <chrono>
#include <thread>

namespace clipod
{

struct buffer_t
{
    size_t m_currentFrame;
    size_t m_remainingFrames;

    std::vector<uint8_t> m_data;

    ALsizei callback (void *data_, ALsizei size_);
    std::chrono::duration<float> currentTimePoint (ALint sampleRate_) const;
    std::chrono::duration<float> remainingTimePoint (ALint sampleRate_) const;
    float percentRemaining () const;
};

class soundBuffer_t
{
public:
    ~soundBuffer_t ();
    soundBuffer_t (muuid::uuid const &trackId_);

    muuid::uuid const &id () const;

    std::chrono::duration<float> const &timePoint () const;

    void play ();
    void pause ();

    void seek (int seek_);
    void seek (std::chrono::duration<float> seek_);

    void load ();
    void stop ();

private:
    ALsizei bufferCallback (void *data_, ALsizei size_);

    muuid::uuid m_id;
    std::filesystem::path m_path;

    ALuint m_alSource;
    ALuint m_alBuffer;
    ALint m_sampleRate;

    std::chrono::duration<float> m_timePoint;

    size_t m_currentFrame;
    std::shared_ptr<buffer_t> m_data;

    std::jthread m_decoderThread;
    std::shared_ptr<decoder_t> m_decoder;
};

} // namespace clipod
