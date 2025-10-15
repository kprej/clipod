#include "clipod/soundBuffer.h"
#include "clipod/brain.h"
using namespace clipod;

#include <spdlog/spdlog.h>

#include <mp4/mp4file.h>
#include <tpropertymap.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
#include <libavutil/log.h>
#include <libavutil/mem.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

#include <AL/al.h>
#include <AL/alext.h>

#include <memory>
#include <optional>
#include <ranges>

AVSampleFormat OUT_FORMAT = AV_SAMPLE_FMT_S16;
ALenum AL_OUT_FORMAT = AL_FORMAT_STEREO16;

ALsizei buffer_t::callback (void *data_, ALsizei size_)
{
    ALsizei read = 0;

    if (m_remainingFrames < size_)
    {
        read = m_remainingFrames;
    }
    else
    {
        read = size_;
    }

    auto output = std::views::counted (static_cast<uint8_t *> (data_), read);

    for (auto &i : output)
    {
        i = m_data[m_currentFrame];
        ++m_currentFrame;
    }

    m_remainingFrames -= read;

    return read;
}

std::chrono::duration<float> buffer_t::currentTimePoint (ALint sampleRate_) const
{
    return std::chrono::duration<float> (
        (((float)m_currentFrame) / ((float)sampleRate_)) / 4.f);
}

std::chrono::duration<float> buffer_t::remainingTimePoint (ALint sampleRate_) const
{
    return std::chrono::duration<float> (
        (((float)m_remainingFrames) / ((float)sampleRate_)) / 4.f);
}

float buffer_t::percentRemaining () const
{
    return (float)m_remainingFrames / (float)m_data.size ();
}

////////////////////////////////////////////////////////////////////////////////
soundBuffer_t::~soundBuffer_t ()
{
    stop ();
}

soundBuffer_t::soundBuffer_t (muuid::uuid const &trackId_)
    : m_id (trackId_)
    , m_alSource (0)
    , m_alBuffer (0)
    , m_sampleRate (0)
    , m_timePoint ()
    , m_currentFrame (0)
    , m_data (std::make_shared<buffer_t> ())
    , m_decoderThread (std::nullopt)
    , m_decoder ()
{
    auto db = brain_t::bb ()->db;
    auto query = db->soundSource (m_id);

    m_sampleRate = query.sampleRate;
    m_path = query.path;

    m_data->m_currentFrame = 0;
    m_data->m_remainingFrames = 0;
}

muuid::uuid const &soundBuffer_t::id () const
{
    return m_id;
}

std::chrono::duration<float> const &soundBuffer_t::timePoint () const
{
    return m_timePoint;
}

ALsizei soundBuffer_t::bufferCallback (void *data_, ALsizei size_)
{
    auto read = m_data->callback (data_, size_);

    if (m_data->percentRemaining () >= 10.f)
        nearEOT ();

    return read;
}

void soundBuffer_t::play ()
{
    m_timePoint = m_data->currentTimePoint (m_sampleRate);
    alSourcePlay (m_alSource);
}

void soundBuffer_t::pause ()
{
    alSourceStop (m_alSource);
}

void soundBuffer_t::seek (int seek_)
{
    auto framesSkipped = 4 * seek_ * m_sampleRate;

    if (m_data->m_currentFrame + framesSkipped <= 0)
        m_data->m_currentFrame = 0;
    else if (m_data->m_currentFrame + framesSkipped <= m_data->m_data.size ())
        m_data->m_currentFrame = m_data->m_data.size () - 1;
    else
        m_data->m_currentFrame += framesSkipped;

    m_timePoint = m_data->remainingTimePoint (m_sampleRate);
}

void soundBuffer_t::load ()
{
    m_decoder = std::make_unique<decoder_t> (m_data);
    m_decoder->setPath (m_path);
    m_decoderThread = std::thread (&decoder_t::decodeFrames, m_decoder.get ());

    alGenBuffers (1, &m_alBuffer);
    alGenSources (1, &m_alSource);

    auto alBufferCallbackSOFT = LPALBUFFERCALLBACKSOFT {};

    alBufferCallbackSOFT = reinterpret_cast<LPALBUFFERCALLBACKSOFT> (
        alGetProcAddress ("alBufferCallbackSOFT"));

    auto callback = [] (void *userptr, void *data, ALsizei size) noexcept -> ALsizei
    { return static_cast<soundBuffer_t *> (userptr)->bufferCallback (data, size); };

    alBufferCallbackSOFT (m_alBuffer, AL_FORMAT_STEREO16, m_sampleRate, callback, this);

    alSourcei (m_alSource, AL_BUFFER, static_cast<ALint> (m_alBuffer));

    if (alGetError () != AL_NO_ERROR)
    {
        spdlog::error ("{}", alGetString (alGetError ()));
    }
}

void soundBuffer_t::stop ()
{
    m_currentFrame = 0;

    alSourceStop (m_alSource);

    m_timePoint = std::chrono::milliseconds (0);

    alDeleteSources (1, &m_alSource);
    alDeleteBuffers (1, &m_alBuffer);

    if (m_decoderThread && m_decoderThread->joinable ())
        m_decoderThread->join ();

    m_decoder.reset ();
}

void soundBuffer_t::unload ()
{
    if (m_decoderThread && m_decoderThread->joinable ())
        m_decoderThread->join ();
}
