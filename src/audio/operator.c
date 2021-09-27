#include "operator.h"
#include "pcm.h"
#include <esp_log.h>

#define TAG "operator"

uint16_t sin_tab[OP_TAB_SIZE];
uint16_t tri_tab[OP_TAB_SIZE];

void operator_init(void) {
    for(int i = 0; i < OP_TAB_SIZE; i++) {
        float angle = M_PI_2 * (float)i / (float)OP_TAB_SIZE;
        sin_tab[i] = sin(angle) * 32767;
    }
    ESP_LOGI(TAG, "sine table generated (%i samples)", OP_TAB_SIZE);

    for(int i = 0; i < OP_TAB_SIZE; i++)
        tri_tab[i] = 32767 * i / OP_TAB_SIZE;
    ESP_LOGI(TAG, "triangle table generated (%i samples)", OP_TAB_SIZE);
}

uint16_t operator_process(operator_t* op) {
    if(op->max_phase == 0)
        return 0;

    op->phase %= op->max_phase;
    int32_t sample = 0;

    switch(op->waveform) {
        case operator_waveform_sine:
        case operator_waveform_triangle: {
            uint32_t lut_pos = 4 * OP_TAB_SIZE * op->phase / op->max_phase;
            uint8_t* table = (op->waveform == operator_waveform_sine) ? sin_tab : tri_tab;
            // move the waveform around
            if(lut_pos < OP_TAB_SIZE) { // 0 <= phase < pi/2
                sample = 32768 + table[lut_pos];
            } else if(lut_pos < OP_TAB_SIZE * 2) { // pi/2 <= phase < pi
                sample = 32768 + table[(2 * OP_TAB_SIZE) - 1 - lut_pos];
            } else if(lut_pos < OP_TAB_SIZE * 3) { // pi <= phase < 3pi/2
                sample = 32768 - table[lut_pos - (2 * OP_TAB_SIZE)];
            } else { // 3pi/2 <= phase < 2pi
                sample = 32768 - table[(4 * OP_TAB_SIZE) - 1 - lut_pos];
            }
            break;
        }

        case operator_waveform_square:
            sample = (2 * op->phase / op->max_phase) * 65535;
            break;

        case operator_waveform_noise:
            sample = rand() & 65535;
            break;
    }

    sample = ((sample - 32768) * op->vol / 65536) + 32768;

    op->phase++;
    return sample;
}

void operator_set_fq(operator_t* op, float fq) {
    if(fq == 0)
        return;

    op->freq = fq;
    op->max_phase = PCM_SAMPLE_RATE / fq;
    // op->phase = 0;
}