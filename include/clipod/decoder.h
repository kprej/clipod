#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#include <filesystem>
#include <mutex>
#include <queue>

namespace clipod
{
struct buffer_t;

class decoder_t
{
public:
    ~decoder_t ();
    decoder_t (std::shared_ptr<buffer_t> buffer_);

    void setPath (std::filesystem::path const &path_);

    void decodeFrames ();
    void stop ();
    bool shouldStop ();
    bool running ();
    size_t remainingFrames ();
    std::queue<AVFrame *> m_frames;

    mutable std::mutex m_mutex;
    std::shared_ptr<buffer_t> buffer;

private:
    void processFrame ();

    std::mutex m_stopMutex;
    bool m_stop;

    bool m_running;
    std::filesystem::path m_path;
    int m_streamIndex;
    AVFormatContext *m_formatContext;
    AVStream *m_stream;
    AVCodecContext *m_decoderContext;
    AVPacket *m_packet = nullptr;
    SwrContext *m_swrContext = swr_alloc ();

    int dst_rate;
    AVChannelLayout dst_ch_layout = AV_CHANNEL_LAYOUT_STEREO;
    enum AVSampleFormat dst_sample_fmt = AV_SAMPLE_FMT_S16;

    std::mutex m_framesMutex;
    size_t m_dataPos;
};
} // namespace clipod
