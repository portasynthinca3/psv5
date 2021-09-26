#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// samples per quarter of a sine/tri wave
#define OP_TAB_SIZE 1024

typedef enum {
    operator_waveform_sine,
    operator_waveform_triangle,
    operator_waveform_square
} operator_waveform_t;

typedef struct {
    operator_waveform_t waveform;
    uint32_t phase;
    float freq;
    uint32_t max_phase;
} operator_t;

void    operator_init   (void);
uint8_t operator_process(operator_t* op);
void    operator_set_fq (operator_t* op, float fq);