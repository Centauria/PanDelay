#include <stdio.h>

#include "PanDelay.h"
#include "ff.h"

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("usage: PanDelay [wav file]\n");
        return 1;
    }
    int err;
    float* buf = NULL;
    int linesize;
    err = av_samples_alloc((uint8_t**)(&buf), &linesize, 1, BUF_SIZE, AV_SAMPLE_FMT_FLT, 0);
    err = av_samples_set_silence((uint8_t**)(&buf), 0, BUF_SIZE, 1, AV_SAMPLE_FMT_FLT);

    // AVAudioFifo* buf = av_audio_fifo_alloc(AV_SAMPLE_FMT_FLT, 1, BUF_SIZE);

    AVFormatContext* context = NULL;
    context = avformat_alloc_context();
    if (!context) {
        EXIT_ON_ERROR(AVERROR(ENOMEM));
    }
    err = avformat_open_input(&context, argv[1], NULL, NULL);
    EXIT_ON_ERROR(err);
    avformat_find_stream_info(context, NULL);
    printf("filename: %s\n", context->url);
    printf("nb_streams: %u\n", context->nb_streams);
    printf("iformat name: %s\n", context->iformat->name);

    printf("Detecting audio stream...\n");
    int stream_index = -1;
    for (int i = 0; i < context->nb_streams; i++) {
        if (context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            stream_index = i;
            break;
        }
    }
    printf("stream index: %d\n", stream_index);

    AVCodecContext* avctx = avcodec_alloc_context3(NULL);
    err = avcodec_parameters_to_context(avctx, context->streams[stream_index]->codecpar);
    EXIT_ON_ERROR(err);
    AVCodec* codec = avcodec_find_decoder(avctx->codec_id);
    err = avcodec_open2(avctx, codec, NULL);
    EXIT_ON_ERROR(err);
    printf("sample format: %s\n", av_get_sample_fmt_name(avctx->sample_fmt));
    if (avctx->channels != 2) {
        printf("num_channels is not 2, abort.");
        EXIT_ON_ERROR(-1);
    }

    AVPacket* pack = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    int read_eof = 0;
    int read_n = 0;
    while (read_eof != 1) {
        err = av_read_frame(context, pack);
        if (pack->stream_index != stream_index) {
            av_packet_unref(pack);
            if (err == AVERROR_EOF) read_eof = 1;
            continue;
        }
        if (err == AVERROR_EOF) {
            avcodec_send_packet(avctx, pack);
        }
        else if (err == 0) {
            while (avcodec_send_packet(avctx, pack) == AVERROR(EAGAIN));
        }
        read_eof = read_frame(avctx, frame, NULL);
        av_packet_unref(pack);
        read_n++;
        printf("read frame: %d\r", read_n);
    }
    av_frame_free(frame);
    av_packet_unref(pack);
    av_packet_free(&pack);
    avcodec_free_context(&avctx);
    avformat_close_input(&context);
    // av_audio_fifo_free(buf);
    av_free(buf);
}