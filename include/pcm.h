#include <stdlib.h>
#include <stdint.h>

#define PCM_I2S_NUM     0
#define PCM_SAMPLE_RATE 48000

typedef struct __attribute__((packed)) {
    uint8_t left;
    uint8_t right;
} pcm_sample_t;

size_t _pcm_buf_stretch(uint8_t* dest, pcm_sample_t* src, size_t len);

void   pcm_init (void);
size_t pcm_write(pcm_sample_t* src, size_t len);