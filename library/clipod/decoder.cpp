#include "clipod/decoder.h"
#include "clipod/soundBuffer.h"

#include "libavcodec/avcodec.h"
#include "libavcodec/packet.h"
using namespace clipod;

extern "C"
{
#include <libavutil/avutil.h>
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
#include <libavutil/log.h>
#include <libavutil/mem.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
}

#include <spdlog/spdlog.h>

decoder_t::~decoder_t ()
{
    while (!m_frames.empty ())
    {
        auto frame = m_frames.front ();
        m_frames.pop ();
        av_frame_free (&frame);
    }
    avcodec_free_context (&m_decoderContext);

    swr_free (&m_swrContext);
    av_packet_free (&m_packet);
}

decoder_t::decoder_t (std::shared_ptr<buffer_t> buffer_)
    : m_mutex ()
    , buffer (buffer_)
    , m_stop (false)
    , m_running (true)
    , m_path ()
    , m_streamIndex (0)
    , m_formatContext (nullptr)
    , m_stream (nullptr)
    , m_decoderContext (nullptr)
    , m_dataPos (0)
{
}

void decoder_t::setPath (std::filesystem::path const &path_)
{
    auto now = std::chrono::steady_clock::now ();
    if (!m_path.empty ())
    {
        return;
    }

    m_path = path_;

    auto ret =
        avformat_open_input (&m_formatContext, m_path.string ().data (), NULL, NULL);

    if (ret < 0)
    {
        spdlog::error ("failed to open the media file {}", m_path.string ());
        assert (false);
    }

    ret = avformat_find_stream_info (m_formatContext, NULL);
    if (ret < 0)
    {
        spdlog::error ("failed to stream info");
        assert (false);
    }

    int m_streamIndex =
        av_find_best_stream (m_formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (m_streamIndex < 0)
    {
        spdlog::error ("no audio stream found in {}", m_path.string ());
        assert (false);
    }

    m_stream = m_formatContext->streams[m_streamIndex];

    const AVCodec *decoder = avcodec_find_decoder (m_stream->codecpar->codec_id);
    if (!decoder)
    {
        spdlog::error ("no decoder found");
        assert (false);
    }

    m_decoderContext = avcodec_alloc_context3 (decoder);

    avcodec_parameters_to_context (m_decoderContext, m_stream->codecpar);

    ret = avcodec_open2 (m_decoderContext, decoder, NULL);
    if (ret < 0)
    {
        spdlog::error ("failed to open the decoder");
        assert (false);
    }

    dst_rate = m_stream->codecpar->sample_rate;
    dst_sample_fmt = AV_SAMPLE_FMT_S16;

    av_opt_set_chlayout (m_swrContext, "in_chlayout", &m_stream->codecpar->ch_layout, 0);
    av_opt_set_int (m_swrContext, "in_sample_rate", m_stream->codecpar->sample_rate, 0);
    av_opt_set_sample_fmt (m_swrContext,
                           "in_sample_fmt",
                           static_cast<AVSampleFormat> (m_stream->codecpar->format),
                           0);

    av_opt_set_chlayout (m_swrContext, "out_chlayout", &dst_ch_layout, 0);
    av_opt_set_int (m_swrContext, "out_sample_rate", dst_rate, 0);
    av_opt_set_sample_fmt (m_swrContext, "out_sample_fmt", dst_sample_fmt, 0);

    /* initialize the resampling context */
    if ((ret = swr_init (m_swrContext)) < 0)
    {
        spdlog::debug ("Could not init resampler context");
        return;
    }
    size_t data_size = 0;

    while (ret >= 0)
    {
        if (m_packet)
            av_packet_free (&m_packet);

        m_packet = av_packet_alloc ();
        ret = av_read_frame (m_formatContext, m_packet);
        if (ret < 0 && (ret != AVERROR (EAGAIN)))
        {
            continue;
        }

        if (m_packet->stream_index != m_streamIndex)
        {
            continue;
        }

        ret = avcodec_send_packet (m_decoderContext, m_packet);
        if (ret < 0 && (ret != AVERROR (EAGAIN)))
        {
            spdlog::debug ("ERROR: failed to decode a frame");
            av_packet_free (&m_packet);
            return;
        }

        auto frame = av_frame_alloc ();
        ret = avcodec_receive_frame (m_decoderContext, frame);
        if (ret < 0 && (ret != AVERROR (EAGAIN)))
        {
            av_frame_free (&frame);
            ret = 0;
            continue;
        }

        while (ret == 0)
        {
            AVFrame *cpFrame = nullptr;

            cpFrame = av_frame_clone (frame);

            m_frames.push (cpFrame);

            av_frame_free (&frame);
            av_free (frame);

            data_size +=
                av_samples_get_buffer_size (NULL,
                                            cpFrame->ch_layout.nb_channels,
                                            cpFrame->nb_samples,
                                            static_cast<AVSampleFormat> (cpFrame->format),
                                            0);

            frame = av_frame_alloc ();
            ret = avcodec_receive_frame (m_decoderContext, frame);
        }
        if (frame)
        {
            av_frame_free (&frame);

            ret = 0;
        }
    }

    if (static_cast<AVSampleFormat> (m_stream->codecpar->format) != AV_SAMPLE_FMT_S16)
        data_size /= 2;

    buffer->m_data.resize (data_size);
    buffer->m_remainingFrames = data_size;
    avformat_close_input (&m_formatContext);
}

void decoder_t::decodeFrames ()
{
    int ret;
    while (true)
    {
        if (shouldStop ())
            break;

        AVFrame *frame = nullptr;
        {
            std::lock_guard<std::mutex> frameLock (m_framesMutex);
            if (m_frames.empty ())
                break;

            frame = m_frames.front ();
            m_frames.pop ();
        }

        AVFrame *resampled_frame = av_frame_alloc ();
        resampled_frame->sample_rate = dst_rate;
        resampled_frame->ch_layout = dst_ch_layout;
        resampled_frame->format = dst_sample_fmt;

        ret = swr_convert_frame (m_swrContext, resampled_frame, frame);

        auto data_size = av_samples_get_buffer_size (
            NULL,
            resampled_frame->ch_layout.nb_channels,
            resampled_frame->nb_samples,
            static_cast<AVSampleFormat> (resampled_frame->format),
            0);

        for (auto i = 0; i < data_size; ++i)
            buffer->m_data[m_dataPos + i] = resampled_frame->data[0][i];

        m_dataPos += data_size;

        av_frame_free (&frame);
        av_free (frame);

        av_frame_free (&resampled_frame);
        av_free (resampled_frame);
    }

    avcodec_free_context (&m_decoderContext);
    av_free (m_decoderContext);
    swr_free (&m_swrContext);
    av_free (m_swrContext);

    std::lock_guard<std::mutex> lock (m_stopMutex);
    m_running = false;
}

void decoder_t::stop ()
{
    std::lock_guard<std::mutex> lock (m_stopMutex);
    m_stop = true;
}

bool decoder_t::shouldStop ()
{
    std::lock_guard<std::mutex> lock (m_stopMutex);
    return m_stop;
}

bool decoder_t::running ()
{
    std::lock_guard<std::mutex> lock (m_stopMutex);
    return m_running;
}

size_t decoder_t::remainingFrames ()
{
    std::lock_guard<std::mutex> frameLock (m_framesMutex);
    return m_frames.size ();
}

void decoder_t::processFrame ()
{
}
