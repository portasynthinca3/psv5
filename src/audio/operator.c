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

void operator_set_fq(operator_t* op, uint16_t fq) {
    if(fq == 0)
        return;

    op->freq = fq;
    float samples_per_cycle = (float)PCM_SAMPLE_RATE / fq;
    op->phase_increment = samples_per_cycle * (float)(1 << OP_FP_BITS);
}

// UNREADABLE CODE AHEAD
//
// why?
// to avoid brances in speed-sensitive code to squeeze out 94 simultaneous sine
// waves even though the code that uses branches works with 79 fine and is way
// more readable
void operator_set_form(operator_t* op, operator_waveform_t form) {
    op->waveform = form;
    int16_t (*table[])(operator_t* op) = {
        &_operator_process_sine, &_operator_process_triangle,
        &_operator_process_square, &_operator_process_noise
    };
    op->process = table[form];
}

#define PROCESSOR_PROLOGUE op->phase %= OP_TAB_SIZE << OP_FP_BITS; int32_t sample = 0;
#define PROCESSOR_EPILOGUE op->phase += op->phase_increment; return sample * op->vol / 32768;

int16_t _operator_process_sine(operator_t* op) {
    PROCESSOR_PROLOGUE
    sample = sin_tab[op->phase >> OP_FP_BITS];
    PROCESSOR_EPILOGUE
}

int16_t _operator_process_triangle(operator_t* op) {
    PROCESSOR_PROLOGUE
    sample = tri_tab[op->phase >> OP_FP_BITS];
    PROCESSOR_EPILOGUE
}

int16_t _operator_process_square(operator_t* op) {
    PROCESSOR_PROLOGUE
    if(op->phase < OP_TAB_SIZE / 2)
        sample = 32767;
    else
        sample = -32768;
    PROCESSOR_EPILOGUE
}

int16_t _operator_process_noise(operator_t* op) {
    PROCESSOR_PROLOGUE
    sample = rand() & 65535;
    PROCESSOR_EPILOGUE
}