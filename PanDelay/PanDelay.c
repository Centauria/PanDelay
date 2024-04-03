#include "PanDelay.h"

void linebuf_free(LineBuf* buf)
{
    free(buf->data);
    free(buf);
}

LineBuf* linebuf_alloc(size_t size)
{
    LineBuf* buf = (LineBuf*)malloc(sizeof(LineBuf));
    if (!buf) return NULL;
    buf->data = (float*)malloc(size * sizeof(float));
    if (buf->data == NULL) {
        free(buf->data);
        return NULL;
    }
    memset(buf->data, 0, size * sizeof(float));
    buf->size = size;
    buf->w = 0;
    return buf;
}

float linebuf_read(LineBuf* buf, int offset)
{
    if (offset <= 0) {
        return buf->data[m_add(buf->w, offset, buf->size)];
    }
    return 0.0f;
}

size_t m_add(size_t src, int offset, size_t size)
{
    int64_t v = (int64_t)src + offset;
    while (v < 0) v += size;
    return (size_t)v % size;
}
