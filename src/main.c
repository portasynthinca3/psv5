#include <esp_log.h>
#include <stdio.h>
#include "pcm.h"
#include "operator.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>
#include <esp_log.h>

void app_main() {
    operator_init();
    pcm_init();

    operator_t op;
    op.vol = 32768;
    operator_set_fq(&op, 200);
    operator_set_form(&op, operator_waveform_sine);
    int64_t len = 100 * PCM_SAMPLE_RATE;
    int64_t elapsed = esp_timer_get_time();
    for(int i = 0; i < len; i++)
        op.process(&op);
    elapsed = esp_timer_get_time() - elapsed;

    ESP_LOGI("main", "calculated %lli seconds of audio in %lli us", len / PCM_SAMPLE_RATE, elapsed);
    ESP_LOGI("main", "%lli samples per second", len * 1000000 / elapsed);
    ESP_LOGI("main", "%lli simultaneous sine waves", (len / PCM_SAMPLE_RATE) * 1000000 / elapsed);
    ESP_LOGI("main", "%lli cpu cycles/sample", 240 * elapsed / len);


    elapsed = esp_timer_get_time();
    len = 1000000;
    for(int i = 0; i < len; i++)
        operator_set_fq(&op, 100);
    elapsed = esp_timer_get_time() - elapsed;

    ESP_LOGI("main", "calculated %lli fq changes in %lli us", len, elapsed);
    ESP_LOGI("main", "%lli changes per second", len * 1000000 / elapsed);
    ESP_LOGI("main", "%lli cpu cycles/change", 240 * elapsed / len);
}