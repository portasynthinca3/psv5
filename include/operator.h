#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define OP_TAB_SIZE 1024 // samples per sine/tri wave
#define OP_FP_BITS 8     // phase decimal part bits

typedef enum {
    operator_waveform_sine,
    operator_waveform_triangle,
    operator_waveform_square,
    operator_waveform_noise
} operator_waveform_t;

typedef struct {
    operator_waveform_t waveform;
    uint32_t phase, phase_increment;
    float freq;
    uint16_t vol;
} operator_t;

void    operator_init   (void);
int16_t operator_process(operator_t* op);
void    operator_set_fq (operator_t* op, uint16_t fq);