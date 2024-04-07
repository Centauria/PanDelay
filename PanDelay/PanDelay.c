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

void linebuf_write(LineBuf* buf, float val)
{
    buf->data[buf->w] = val;
    (buf->w)++;
    if (buf->w == buf->size) buf->w = 0;
}
