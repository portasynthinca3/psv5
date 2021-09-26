#include "pcm.h"
#include <stdlib.h>
#include <stdint.h>
#include <driver/i2s.h>
#include <freertos/FreeRTOS.h>

uint8_t* stretched_buf = NULL;
size_t stretched_buf_size = 0;

void pcm_init(void) {
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN | I2S_MODE_ADC_BUILT_IN,
        .sample_rate =  PCM_SAMPLE_RATE,
        .bits_per_sample = 16,
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .intr_alloc_flags = 0,
        .dma_buf_count = 2,
        .dma_buf_len = 1024,
        .use_apll = 1,
    };
    i2s_driver_install(PCM_I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);

    pcm_sample_t hi[512];
    pcm_sample_t lo[512];
    for(int i = 0; i < 512; i++) {
        hi[i].left = 255;
        hi[i].right = 255;
        lo[i].left = 0;
        lo[i].right = 0;
    }

    while(true) {
        pcm_write(hi, 512);
        pcm_write(lo, 512);
    }
}

size_t pcm_write(pcm_sample_t* src, size_t len) {
    // prepare buffer for stretched data
    size_t data_len = len * 4;
    if(stretched_buf_size < data_len) {
        stretched_buf = realloc(stretched_buf, data_len);
        stretched_buf_size = data_len;
    }

    size_t written;
    _pcm_buf_stretch(stretched_buf, src, len);
    i2s_write(PCM_I2S_NUM, stretched_buf, data_len, &written, portMAX_DELAY);
    return written;
}

/*
 * Stretches input sample data into an array that can be fed to I2S
 * 
 * Input:
 * +------------------
 * |LR|LR|LR|LR| . . .
 * +------------------
 * 
 * Output:
 * +--------------------------
 * | R L| R L| R L| R L| . . .
 * +--------------------------
 */
size_t _pcm_buf_stretch(uint8_t* dest, pcm_sample_t* src, size_t len) {
    for(int i = 0; i < len; i++) {
        dest[(i * 4) + 1] = src[i].right;
        dest[(i * 4) + 3] = src[i].left;
    }
    return len * 4;
}