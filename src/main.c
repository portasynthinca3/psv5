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
    operator_set_fq(&op, 1000);
    int64_t elapsed = esp_timer_get_time();
    int64_t len = 100 * PCM_SAMPLE_RATE;
    for(int i = 0; i < len; i++)
        operator_process(&op);
    elapsed = esp_timer_get_time() - elapsed;

    ESP_LOGI("main", "calculated %lli seconds of audio in %lli us", len / PCM_SAMPLE_RATE, elapsed);
    ESP_LOGI("main", "%lli samples per second", len * 1000000 / elapsed);
    ESP_LOGI("main", "%lli simultaneous sine waves", (len / PCM_SAMPLE_RATE) * 1000000 / elapsed);
    ESP_LOGI("main", "%lli cycles/sample", 240 * elapsed / len);
}