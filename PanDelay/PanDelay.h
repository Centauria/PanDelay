#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BUF_SIZE 131072

typedef struct linebuf {
	float* data;
	size_t size;
	size_t w;
} LineBuf;

void linebuf_free(LineBuf* buf);
LineBuf* linebuf_alloc(size_t size);
float linebuf_read(LineBuf* buf, int offset);
void linebuf_write(LineBuf* buf, size_t offset);

size_t m_add(size_t src, int offset, size_t size);