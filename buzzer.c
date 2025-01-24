#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "buzzer.h"

#define BUZZER_PIN 26

void init_buzzer() {
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_wrap(slice_num, 65535);
    pwm_set_enabled(slice_num, true);
}

void play_tone(uint frequency, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint clock = 125000000;
    uint divider = clock / frequency / 65535;
    pwm_set_clkdiv(slice_num, divider);
    pwm_set_gpio_level(BUZZER_PIN, 32768);
    sleep_ms(duration_ms);
    pwm_set_gpio_level(BUZZER_PIN, 0);
}


