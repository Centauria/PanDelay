#include "ff.h"
#include "PanDelay.h"

int read_frame(AVCodecContext* avctx, SwrContext* swctx, AVFrame* frame, uint8_t** buf) {
    int err;
    while (1) {
        err = avcodec_receive_frame(avctx, frame);
        if (err == AVERROR(EAGAIN)) break;
        else if (err == AVERROR_EOF) {
            return -1;
        }
        else if (err >= 0) {
            return swr_convert(swctx, buf, BUF_SIZE * sizeof(float), (const uint8_t**)frame->data, frame->nb_samples);
        }
        else {
            EXIT_ON_ERROR(err);
        }
    }
    return 0;
}

int write_frame(AVCodecContext* avctx, SwrContext* swctx, AVFrame* frame, uint8_t** buf, int nb_samples)
{
    int err;
    av_frame_get_buffer(frame, 0);
    av_frame_make_writable(frame);
    av_samples_copy(frame->extended_data, buf, 0, 0, nb_samples, 2, avctx->sample_fmt);
    float** y = (float**)(buf);
    printf("input samples: %d\n", nb_samples);
    printf("frame samples: %d\n", frame->nb_samples);
    printf("frame contents: \n");
    printf("  [%f, %f, ... %f]\n", y[0][0], y[0][1], y[0][frame->nb_samples - 1]);
    printf("  [%f, %f, ... %f]\n", y[1][0], y[1][1], y[1][frame->nb_samples - 1]);
    return 0;
}
