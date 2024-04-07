#pragma once
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/audio_fifo.h>
#include <libswresample/swresample.h>

#define EXIT_ON_ERROR(e) if(e){fprintf(stderr,"ERROR code: %d\n",(e));return (e);}

int read_frame(AVCodecContext* avctx, SwrContext* swctx, AVFrame* frame, float* buf);