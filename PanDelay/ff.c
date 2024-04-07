#include "ff.h"
#include "PanDelay.h"

int read_frame(AVCodecContext* avctx, SwrContext* swctx, AVFrame* frame, float* buf) {
    int err;
    while (1) {
        err = avcodec_receive_frame(avctx, frame);
        if (err == AVERROR(EAGAIN)) break;
        else if (err == AVERROR_EOF) {
            return 1;
        }
        else if (err >= 0) {
            // printf("nb_samples: %d\n", frame->nb_samples);
        }
        else {
            EXIT_ON_ERROR(err);
        }
    }
    return 0;
}