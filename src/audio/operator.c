#include "operator.h"
#include "pcm.h"
#include <esp_log.h>

#define TAG "operator"

int16_t sin_tab[OP_TAB_SIZE];
int16_t tri_tab[OP_TAB_SIZE];

void operator_init(void) {
    for(int i = 0; i < OP_TAB_SIZE; i++) {
        float angle = 2 * M_PI * (float)i / (float)OP_TAB_SIZE;
        sin_tab[i] = sin(angle) * 32767;
    }
    ESP_LOGI(TAG, "sine table generated (%i samples)", OP_TAB_SIZE);

    for(int i = 0; i < OP_TAB_SIZE; i++)
        tri_tab[i] = 32767 * i / OP_TAB_SIZE;
    ESP_LOGI(TAG, "triangle table generated (%i samples)", OP_TAB_SIZE);
}

int16_t operator_process(operator_t* op) {
    op->phase %= OP_TAB_SIZE << OP_FP_BITS;
    int32_t sample = 0;

    switch(op->waveform) {
        case operator_waveform_sine:
        case operator_waveform_triangle: {
            uint32_t lut_pos = op->phase >> OP_FP_BITS;
            int16_t* table = (op->waveform == operator_waveform_sine) ? sin_tab : tri_tab;
            sample = table[lut_pos];
            break;
        }

        case operator_waveform_square:
            if(op->phase < OP_TAB_SIZE / 2)
                sample = 32767;
            else
                sample = -32768;
            break;

        case operator_waveform_noise:
            sample = rand() & 65535;
            break;
    }

    sample = sample * op->vol / 32768;

    op->phase += op->phase_increment;
    return sample;
}

void operator_set_fq(operator_t* op, uint16_t fq) {
    if(fq == 0)
        return;

    op->freq = fq;
    float samples_per_cycle = (float)PCM_SAMPLE_RATE / fq;
    op->phase_increment = samples_per_cycle * (float)(1 << OP_FP_BITS);
    // op->phase = 0;
}