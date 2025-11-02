#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#include <filesystem>
#include <queue>
#include <thread>

namespace clipod
{
struct buffer_t;

class decoder_t
{
public:
    ~decoder_t ();
    decoder_t (std::shared_ptr<buffer_t> buffer_);

    void setPath (std::filesystem::path const &path_);

    void decodeFrames (std::stop_token stoken_);
    bool running ();
    size_t remainingFrames ();

private:
    void processFrame ();

    std::queue<AVFrame *> m_frames;

    std::shared_ptr<buffer_t> m_buffer;

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

    size_t m_dataPos;
};
} // namespace clipod
