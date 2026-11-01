/* Scalar, owned zlib stream ABI. This bridge is independent Buoy project code. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "zlib.h"
#ifdef _WIN32
#define API __declspec(dllexport)
#else
#define API __attribute__((visibility("default")))
#endif

typedef struct {
    z_stream stream;
    int decoding, live, ended, finished, failed;
    int64_t input_count, output_count, max_input, max_output, max_chunk, max_call_output;
    unsigned char *output;
    size_t output_length, output_capacity;
    char error[256];
} BCKStream;

static int64_t failure(BCKStream *s, const char *message) {
    s->failed = 1;
    snprintf(s->error, sizeof(s->error), "%s", message);
    if (s->live) {
        if (s->decoding) inflateEnd(&s->stream); else deflateEnd(&s->stream);
        s->live = 0;
    }
    free(s->output);
    s->output = NULL;
    s->output_length = s->output_capacity = 0;
    return -1;
}

API void *bck_new(int64_t decoding, int64_t gzip, int64_t level,
                  int64_t max_input, int64_t max_output, int64_t max_chunk, int64_t max_call_output) {
    if ((decoding != 0 && decoding != 1) || (gzip != 0 && gzip != 1) ||
        level < -1 || level > 9 || max_input < 0 || max_output < 0 ||
        max_chunk < 1 || max_chunk > 16777216 || max_call_output < 1 || max_call_output > 16777216) return NULL;
    BCKStream *s = calloc(1, sizeof(*s));
    if (!s) return NULL;
    s->decoding = (int)decoding;
    s->max_input = max_input;
    s->max_output = max_output;
    s->max_chunk = max_chunk;
    s->max_call_output = max_call_output;
    int bits = gzip ? 31 : 15;
    int code = decoding ? inflateInit2(&s->stream, bits)
                        : deflateInit2(&s->stream, (int)level, Z_DEFLATED, bits, 8, Z_DEFAULT_STRATEGY);
    if (code != Z_OK) { free(s); return NULL; }
    s->live = 1;
    return s;
}

static int reserve_output(BCKStream *s, size_t needed) {
    if (needed <= s->output_capacity) return 1;
    size_t capacity = s->output_capacity ? s->output_capacity : 65536;
    while (capacity < needed) {
        if (capacity > 1073741824 / 2) { capacity = needed; break; }
        capacity *= 2;
    }
    /* Allocation is bounded by the remaining total output budget for this call. */
    int64_t prior = s->output_count - (int64_t)s->output_length;
    int64_t maximum64 = s->max_output - prior;
    if (maximum64 > s->max_call_output) maximum64 = s->max_call_output;
    size_t maximum = (size_t)maximum64;
    if (capacity > maximum) capacity = maximum;
    unsigned char *next = realloc(s->output, capacity);
    if (!next) return 0;
    s->output = next;
    s->output_capacity = capacity;
    return 1;
}

/* flush: 0 Write, 2 sync Flush (encoder), 4 Finish. Input is borrowed for this call. */
API int64_t bck_process(void *handle, const void *input, int64_t length, int64_t flush) {
    BCKStream *s = handle;
    if (!s || s->failed) return -1;
    if (s->finished) return failure(s, "Stream has already finished");
    if (length < 0 || length > s->max_chunk || (length && !input)) return failure(s, "Input chunk exceeds maxChunkBytes or is invalid");
    if (length > s->max_input - s->input_count) return failure(s, "Input exceeds maxInputBytes");
    if (flush != 0 && flush != 2 && flush != 4) return failure(s, "Invalid flush mode");
    if (flush && length) return failure(s, "Flush and Finish do not accept input");
    free(s->output);
    s->output = NULL;
    s->output_length = s->output_capacity = 0;
    if (s->decoding && flush == 2) return failure(s, "Decoder does not support Flush");
    if (s->decoding && s->ended) {
        if (length) return failure(s, "Trailing bytes or concatenated members are not accepted");
        if (flush == 4) s->finished = 1;
        return 0;
    }
    if (s->decoding && flush == 4) return failure(s, "Truncated compressed stream: no validated end marker");
    if (!length && flush == 0) return 0;
    s->stream.next_in = (Bytef *)(uintptr_t)input;
    s->stream.avail_in = (uInt)length;
    for (;;) {
        int64_t remaining = s->max_output - s->output_count;
        int64_t call_remaining = s->max_call_output - (int64_t)s->output_length;
        if (remaining > call_remaining) remaining = call_remaining;
        uInt available = remaining > 65536 ? 65536 : (uInt)remaining;
        unsigned char probe;
        if (available) {
            if (!reserve_output(s, s->output_length + available)) return failure(s, "Cannot allocate bounded output buffer");
            s->stream.next_out = s->output + s->output_length;
            s->stream.avail_out = available;
        } else {
            /* Permit a checksum/footer with zero remaining output, but reject another byte. */
            s->stream.next_out = &probe;
            s->stream.avail_out = 1;
        }
        uInt before_input = s->stream.avail_in;
        uInt before_output = s->stream.avail_out;
        int code = s->decoding ? inflate(&s->stream, Z_NO_FLUSH) : deflate(&s->stream, (int)flush);
        uInt produced = before_output - s->stream.avail_out;
        uInt consumed = before_input - s->stream.avail_in;
        s->input_count += consumed;
        if (!available && produced) return failure(s, s->output_count >= s->max_output ? "Output exceeds maxOutputBytes" : "Output exceeds maxCallOutputBytes");
        s->output_count += produced;
        s->output_length += produced;
        if (code == Z_STREAM_END) {
            if (s->stream.avail_in) return failure(s, "Trailing bytes or concatenated members are not accepted");
            s->ended = 1;
            if (!s->decoding) s->finished = 1;
            if (s->decoding) inflateEnd(&s->stream); else deflateEnd(&s->stream);
            s->live = 0;
            break;
        }
        if (code == Z_NEED_DICT) return failure(s, "Preset dictionaries are not supported");
        if (code != Z_OK && code != Z_BUF_ERROR) {
            char message[256];
            snprintf(message, sizeof(message), "zlib failure (%d): %s", code, s->stream.msg ? s->stream.msg : "invalid stream");
            return failure(s, message);
        }
        if (!consumed && !produced) {
            if (!s->stream.avail_in && flush != 4) break;
            return failure(s, "Compression stream made no progress");
        }
        if (flush == 4) continue;
        if (!s->stream.avail_in && s->stream.avail_out) break;
    }
    s->stream.next_in = NULL;
    s->stream.avail_in = 0;
    s->stream.next_out = NULL;
    s->stream.avail_out = 0;
    return (int64_t)s->output_length;
}

API const void *bck_output(void *handle) { return handle ? ((BCKStream *)handle)->output : NULL; }
API const char *bck_error(void *handle) { return handle ? ((BCKStream *)handle)->error : "Closed stream"; }
API int64_t bck_input_count(void *handle) { return handle ? ((BCKStream *)handle)->input_count : 0; }
API int64_t bck_output_count(void *handle) { return handle ? ((BCKStream *)handle)->output_count : 0; }
API int64_t bck_ended(void *handle) { return handle ? ((BCKStream *)handle)->ended : 0; }
API void bck_delete(void *handle) {
    BCKStream *s = handle;
    if (!s) return;
    if (s->live) { if (s->decoding) inflateEnd(&s->stream); else deflateEnd(&s->stream); }
    free(s->output);
    free(s);
}
