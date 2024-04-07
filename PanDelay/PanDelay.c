#include "PanDelay.h"

void linebuf_free(LineBuf* buf)
{
    av_free(buf->data);
    av_free(buf);
}

LineBuf* linebuf_alloc(size_t size)
{
    LineBuf* buf = (LineBuf*)av_malloc(sizeof(LineBuf));
    if (!buf) return NULL;
    buf->data = (float*)av_mallocz(size * sizeof(float));
    if (buf->data == NULL) {
        free(buf->data);
        return NULL;
    }
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

int delay(LineBuf* buf, uint8_t** buf_read, size_t buf_read_size, uint8_t** buf_write, size_t* buf_write_size, size_t sr)
{
    float** x = (float**)buf_read;
    float** y = (float**)buf_write;
    size_t write_size = 0;
    for (int i = 0; i < buf_read_size; i++) {
        float r[2] = {
            buf->data[m_add(buf->w, (int)((offset[0] - 1.0f) * delay_time * sr), buf->size)],
            buf->data[m_add(buf->w, (int)((offset[1] - 1.0f) * delay_time * sr), buf->size)]
        };
        y[0][i] = x[0][i] + feedback * r[0];
        y[1][i] = x[0][i] + feedback * r[1];
        linebuf_write(buf, buf_read[0][i] + feedback * buf->data[m_add(buf->w, -(int)(delay_time * sr), buf->size)]);
        write_size++;
    }
    if (buf_write_size) *buf_write_size = write_size;
    return write_size;
}
