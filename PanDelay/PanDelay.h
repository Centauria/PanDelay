#pragma once

#include <stdint.h>
#include <libavutil/avutil.h>

#define BUF_SIZE 131072

static float delay_time = 0.5f;
static float feedback = 0.7f;
static float offset[2] = { 0.0f, 0.5f };

typedef struct linebuf {
	float* data;
	size_t size;
	size_t w;
} LineBuf;

void linebuf_free(LineBuf* buf);
LineBuf* linebuf_alloc(size_t size);
float linebuf_read(LineBuf* buf, int offset);
void linebuf_write(LineBuf* buf, float val);
int delay(LineBuf* buf, uint8_t** buf_read, size_t buf_read_size, uint8_t** buf_write, size_t* buf_write_size, size_t sr);

inline size_t m_add(size_t src, int offset, size_t size)
{
	int64_t v = (int64_t)src + offset;
	while (v < 0) v += size;
	return (size_t)v % size;
}