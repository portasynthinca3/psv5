#include <esp_log.h>
#include <stdio.h>
#include "pcm.h"
#include "operator.h"

void app_main() {
    operator_init();
    pcm_init();

    operator_t op1, op2;
    op1.waveform = operator_waveform_sine;
    op2.waveform = operator_waveform_square;
    operator_set_fq(&op1, 50);

    pcm_sample_t buf[512];
    while(1) {
        for(int i = 0; i < 512; i++) {
            operator_set_fq(&op2, 1000 + (((int)operator_process(&op1) - 128) / 2));
            buf[i].left = buf[i].right = operator_process(&op2);
        }
        pcm_write(buf, 512);
    }
}