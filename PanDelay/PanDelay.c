#include "PanDelay.h"

void linebuf_free(LineBuf* buf)
{
    free(buf->data);
    free(buf);
}

LineBuf* linebuf_alloc(size_t size)
{
    LineBuf* buf = (LineBuf*)av_malloc(sizeof(LineBuf));
    if (!buf) return NULL;
    buf->data = (float*)av_mallocz(2 * size * sizeof(float));
    if (buf->data == NULL) {
        free(buf->data);
        return NULL;
    }
    buf->size = size;
    buf->w = 0;
    return buf;
}

float linebuf_read(LineBuf* buf, int ch, int offset)
{
    if (offset <= 0) {
        return buf->data[2 * m_add(buf->w, offset, buf->size) + ch];
    }
    return 0.0f;
}

void linebuf_write(LineBuf* buf, int ch, float val)
{
    buf->data[2 * (buf->w) + ch] = val;
    (buf->w)++;
    if (buf->w == buf->size) buf->w = 0;
}
