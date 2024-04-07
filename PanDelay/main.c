#include <stdio.h>

#include "PanDelay.h"
#include "ff.h"

int main(int argc, char** argv)
{
    if (argc != 3) {
        printf("usage: PanDelay [wav file] [output wav file]\n");
        return 1;
    }
    int err;
    uint8_t* buf[2];
    uint8_t* buf_read[1];
    int linesize;
    err = av_samples_alloc(buf, &linesize, 2, BUF_SIZE, AV_SAMPLE_FMT_FLTP, 0);
    err = av_samples_set_silence(buf, 0, BUF_SIZE, 2, AV_SAMPLE_FMT_FLTP);
    err = av_samples_alloc(buf_read, &linesize, 1, BUF_SIZE, AV_SAMPLE_FMT_FLTP, 0);
    err = av_samples_set_silence(buf_read, 0, BUF_SIZE, 1, AV_SAMPLE_FMT_FLTP);

    // AVAudioFifo* buf = av_audio_fifo_alloc(AV_SAMPLE_FMT_FLT, 1, BUF_SIZE);
    // float* buf_input = (float*)av_mallocz(2 * BUF_SIZE * sizeof(float));

    LineBuf* line_buf = linebuf_alloc(BUF_SIZE);

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
    int audio_stream_index = -1;
    for (int i = 0; i < context->nb_streams; i++) {
        if (context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            break;
        }
    }
    printf("stream index: %d\n", audio_stream_index);

    AVFormatContext* output_context = NULL;
    avformat_alloc_output_context2(&output_context, NULL, NULL, argv[2]);
    avio_open(&output_context->pb, argv[2], AVIO_FLAG_READ_WRITE);
    AVCodec* output_codec = avcodec_find_encoder(output_context->oformat->audio_codec);
    AVCodecContext* outctx = avcodec_alloc_context3(output_codec);

    AVCodecContext* avctx = avcodec_alloc_context3(NULL);
    err = avcodec_parameters_to_context(avctx, context->streams[audio_stream_index]->codecpar);
    EXIT_ON_ERROR(err);
    AVCodec* codec = avcodec_find_decoder(avctx->codec_id);
    err = avcodec_open2(avctx, codec, NULL);
    EXIT_ON_ERROR(err);
    printf("sample format: %s\n", av_get_sample_fmt_name(avctx->sample_fmt));
    if (avctx->channels != 1) {
        printf("num_channels is not 1, abort.");
        goto codec_free;
    }
    outctx->bit_rate = 192000;
    outctx->sample_rate = avctx->sample_rate;
    outctx->sample_fmt = AV_SAMPLE_FMT_S16;
    outctx->ch_layout = avctx->ch_layout;
    outctx->channels = 2;
    AVStream* output_stream = avformat_new_stream(output_context, NULL);
    avcodec_parameters_to_context(outctx, output_stream->codecpar);
    avcodec_open2(outctx, output_codec, NULL);
    avformat_write_header(output_context, NULL);
    av_dump_format(output_context, 0, argv[2], 1);

    SwrContext* swctx = NULL;
    swr_alloc_set_opts2(&swctx, &avctx->ch_layout, AV_SAMPLE_FMT_FLT, avctx->sample_rate,
        &avctx->ch_layout, avctx->sample_fmt, avctx->sample_rate,
        0, NULL);
    swr_init(swctx);

    SwrContext* output_swctx = NULL;
    AVChannelLayout layout = AV_CHANNEL_LAYOUT_STEREO;
    swr_alloc_set_opts2(&output_swctx, &layout, AV_SAMPLE_FMT_S16, avctx->sample_rate,
        &avctx->ch_layout, AV_SAMPLE_FMT_FLT, avctx->sample_rate,
        0, NULL);
    swr_init(output_swctx);


    AVPacket* pack = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    AVFrame* output_frame = av_frame_alloc();
    int read_eof = 0;
    int read_n = 0;
    while (read_eof != -1) {
        err = av_read_frame(context, pack);
        if (pack->stream_index != audio_stream_index) {
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
        err = read_frame(avctx, swctx, frame, buf_read);
        if (err <= 0) {
            read_eof = err;
        }
        else {
            delay(line_buf, buf_read, err, buf, NULL, avctx->sample_rate);
            write_frame(avctx, swctx, frame, buf, err);
        }
        av_packet_unref(pack);
        read_n++;
        printf("read frame: %d\r", read_n);
    }
    printf("\nDone!\n");
    av_frame_free(&output_frame);
    av_frame_free(&frame);
    av_packet_unref(pack);
    av_packet_free(&pack);
    // avformat_free_context(&output_stream);
    swr_free(&swctx);
codec_free:    avcodec_free_context(&avctx);
    avcodec_free_context(&outctx);
    avformat_close_input(&context);
    // av_audio_fifo_free(buf);
    linebuf_free(line_buf);
    av_freep(&buf_read[0]);
    av_freep(&buf[0]);
}