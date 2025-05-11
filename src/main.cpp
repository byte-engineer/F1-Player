// ffm.cpp

// Open File -> Find Streams -> Finding Video stream -> Find decoder -> Get codec ctx -> Get sws ctx -> Get RGBA from the ctx -> Display frame


#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>

// FFmpeg headers (C-style)
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./" << argv[0] << " <video_file>" << std::endl;
        return -1;
    }

    const char* input_filename = argv[1];

    // Open input video
    AVFormatContext* fmt_ctx = nullptr;
    if (avformat_open_input(&fmt_ctx, input_filename, nullptr, nullptr) < 0) {
        std::cerr << "Could not open input file." << std::endl;
        return -1;
    }

    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        std::cerr << "Could not find stream information." << std::endl;
        return -1;
    }

    // Finding the video stream.
    int video_stream_index = -1;
    for (unsigned int i = 0; i < fmt_ctx->nb_streams; ++i) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }
    if (video_stream_index == -1) {
        std::cerr << "Could not find a video stream." << std::endl;
        return -1;
    }

    AVCodecParameters* codecpar = fmt_ctx->streams[video_stream_index]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        std::cerr << "Decoder not found." << std::endl;
        return -1;
    }

    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0) {
        std::cerr << "Could not copy codec parameters." << std::endl;
        return -1;
    }

    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        std::cerr << "Could not open codec." << std::endl;
        return -1;
    }

    AVPacket* pkt = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    AVFrame* rgba_frame = av_frame_alloc();

    SwsContext* sws_ctx = sws_getContext(
        codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
        codec_ctx->width, codec_ctx->height, AV_PIX_FMT_RGBA,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );

    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, codec_ctx->width, codec_ctx->height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(num_bytes * sizeof(uint8_t));

    av_image_fill_arrays(
        rgba_frame->data, rgba_frame->linesize, buffer,
        AV_PIX_FMT_RGBA, codec_ctx->width, codec_ctx->height, 1
    );

    bool frame_extracted = false;

    while (av_read_frame(fmt_ctx, pkt) >= 0 && !frame_extracted) {
        if (pkt->stream_index == video_stream_index) {
            if (avcodec_send_packet(codec_ctx, pkt) == 0) {
                if (avcodec_receive_frame(codec_ctx, frame) == 0) {
                    sws_scale(sws_ctx,
                              frame->data, frame->linesize,
                              0, codec_ctx->height,
                              rgba_frame->data, rgba_frame->linesize);

                    frame_extracted = true;
                }
            }
        }
        av_packet_unref(pkt);
    }

    // Display using SFML
sf::RenderWindow window(sf::VideoMode({codec_ctx->width, codec_ctx->height}), "Video Playback");
sf::Texture texture;
sf::Sprite sprite(texture);

sf::Clock clock;
double frame_delay = 1.0 / av_q2d(fmt_ctx->streams[video_stream_index]->r_frame_rate); // seconds per frame

while (av_read_frame(fmt_ctx, pkt) >= 0) {
    if (pkt->stream_index == video_stream_index) {
        if (avcodec_send_packet(codec_ctx, pkt) == 0) {
            while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                // Convert to RGBA
                sws_scale(sws_ctx,
                          frame->data, frame->linesize,
                          0, codec_ctx->height,
                          rgba_frame->data, rgba_frame->linesize);

                // Create image and update texture
                sf::Image image({codec_ctx->width, codec_ctx->height}, rgba_frame->data[0]);
                texture.loadFromImage(image);
                sprite.setTexture(texture, true);

                // Wait to match video frame rate
                while (clock.getElapsedTime().asSeconds() < frame_delay) {
                    sf::sleep(sf::milliseconds(1));
                }
                clock.restart();

                // Handle events
                while (std::optional event = window.pollEvent()) {
                    if (event->is<sf::Event::Closed>())
                        window.close();
                }

                // Display frame
                window.clear();
                window.draw(sprite);
                window.display();
            }
        }
    }
    av_packet_unref(pkt);

    // Exit if window closed
    if (!window.isOpen())
        break;
}
    // Cleanup
    av_free(buffer);
    sws_freeContext(sws_ctx);
    av_frame_free(&frame);
    av_frame_free(&rgba_frame);
    av_packet_free(&pkt);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);

    return 0;
}
