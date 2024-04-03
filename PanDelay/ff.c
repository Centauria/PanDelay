#include "ff.h"

int read_frame(AVCodecContext* avctx, AVFrame* frame, float* buf) {
    int err;
    while (1) {
        err = avcodec_receive_frame(avctx, frame);
        if (err == AVERROR(EAGAIN)) break;
        else if (err == AVERROR_EOF) {
            return 1;
        }
        else if (err >= 0) {
            int data_size = av_get_bytes_per_sample(avctx->sample_fmt);

        }
        else {
            EXIT_ON_ERROR(err);
        }
    }
    return 0;
}