#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define OP_TAB_SIZE 1024 // samples per sine/tri wave
#define OP_FP_BITS 8     // phase decimal part bits

typedef enum {
    operator_waveform_sine = 0,
    operator_waveform_triangle,
    operator_waveform_square,
    operator_waveform_noise
} operator_waveform_t;

typedef struct _operator_s {
    operator_waveform_t waveform;
    uint32_t phase, phase_increment;
    float freq;
    uint16_t vol;
    int16_t (*process)(struct _operator_s* op);
} operator_t;

void operator_init    (void);
void operator_set_fq  (operator_t* op, uint16_t fq);
void operator_set_form(operator_t* op, operator_waveform_t form);

int16_t _operator_process_sine    (operator_t* op);
int16_t _operator_process_triangle(operator_t* op);
int16_t _operator_process_square  (operator_t* op);
int16_t _operator_process_noise   (operator_t* op);