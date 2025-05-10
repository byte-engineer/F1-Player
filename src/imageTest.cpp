extern "C" {
#include <libavutil/mem.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
}

#include <iostream>
#include <vector>

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt, const char *tag) {
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

    std::cout << tag << ": pts:" << av_ts2str(pkt->pts)
              << " pts_time:" << av_ts2timestr(pkt->pts, time_base)
              << " dts:" << av_ts2str(pkt->dts)
              << " dts_time:" << av_ts2timestr(pkt->dts, time_base)
              << " duration:" << av_ts2str(pkt->duration)
              << " duration_time:" << av_ts2timestr(pkt->duration, time_base)
              << " stream_index:" << pkt->stream_index
              << std::endl;
}

int main(int argc, char **argv) {
    const AVOutputFormat *ofmt = nullptr;
    AVFormatContext *ifmt_ctx = nullptr, *ofmt_ctx = nullptr;
    AVPacket *pkt = nullptr;
    const char *in_filename, *out_filename;
    int ret;
    int stream_index = 0;
    int *stream_mapping = nullptr;
    int stream_mapping_size = 0;

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " input output\n"
                  << "API example program to remux a media file with libavformat and libavcodec.\n"
                  << "The output format is guessed according to the file extension.\n"
                  << std::endl;
        return 1;
    }

    in_filename  = argv[1];
    out_filename = argv[2];

    pkt = av_packet_alloc();
    if (!pkt) {
        std::cerr << "Could not allocate AVPacket" << std::endl;
        return 1;
    }

    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, nullptr, nullptr)) < 0) {
        std::cerr << "Could not open input file '" << in_filename << "'" << std::endl;
        goto end;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, nullptr)) < 0) {
        std::cerr << "Failed to retrieve input stream information" << std::endl;
        goto end;
    }

    av_dump_format(ifmt_ctx, 0, in_filename, 0);

    avformat_alloc_output_context2(&ofmt_ctx, nullptr, nullptr, out_filename);
    if (!ofmt_ctx) {
        std::cerr << "Could not create output context" << std::endl;
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    stream_mapping_size = ifmt_ctx->nb_streams;
    stream_mapping = static_cast<int*>(av_calloc(stream_mapping_size, sizeof(*stream_mapping)));
    if (!stream_mapping) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    ofmt = ofmt_ctx->oformat;

    for (int i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVCodecParameters *in_codecpar = in_stream->codecpar;

        if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
            stream_mapping[i] = -1;
            continue;
        }

        stream_mapping[i] = stream_index++;

        AVStream *out_stream = avformat_new_stream(ofmt_ctx, nullptr);
        if (!out_stream) {
            std::cerr << "Failed allocating output stream" << std::endl;
            ret = AVERROR_UNKNOWN;
            goto end;
        }

        ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
        if (ret < 0) {
            std::cerr << "Failed to copy codec parameters" << std::endl;
            goto end;
        }
        out_stream->codecpar->codec_tag = 0;
    }

    av_dump_format(ofmt_ctx, 0, out_filename, 1);

    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            std::cerr << "Could not open output file '" << out_filename << "'" << std::endl;
            goto end;
        }
    }

    ret = avformat_write_header(ofmt_ctx, nullptr);
    if (ret < 0) {
        std::cerr << "Error occurred when opening output file" << std::endl;
        goto end;
    }

    while (true) {
        ret = av_read_frame(ifmt_ctx, pkt);
        if (ret < 0)
            break;

        AVStream *in_stream = ifmt_ctx->streams[pkt->stream_index];

        if (pkt->stream_index >= stream_mapping_size ||
            stream_mapping[pkt->stream_index] < 0) {
            av_packet_unref(pkt);
            continue;
        }

        pkt->stream_index = stream_mapping[pkt->stream_index];
        AVStream *out_stream = ofmt_ctx->streams[pkt->stream_index];
        log_packet(ifmt_ctx, pkt, "in");

        av_packet_rescale_ts(pkt, in_stream->time_base, out_stream->time_base);
        pkt->pos = -1;
        log_packet(ofmt_ctx, pkt, "out");

        ret = av_interleaved_write_frame(ofmt_ctx, pkt);
        if (ret < 0) {
            std::cerr << "Error muxing packet" << std::endl;
            break;
        }
    }

    av_write_trailer(ofmt_ctx);

end:
    av_packet_free(&pkt);
    avformat_close_input(&ifmt_ctx);

    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);

    av_freep(&stream_mapping);

    if (ret < 0 && ret != AVERROR_EOF) {
        std::cerr << "Error occurred: " << av_err2str(ret) << std::endl;
        return 1;
    }

    return 0;
}
