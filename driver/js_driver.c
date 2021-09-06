#include <stdio.h>
#include "js_driver.h"
#include "gpio.h"

void print(int num)
{
    printf("%d\n", num);
}

void toggle_led()
{
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

void set_led(bool is_on)
{
    if (is_on)
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

void delay(int num)
{
    HAL_Delay(num);
}

void js_driver_init(JS *js)
{
    js_set(js, js_glob(js), "print", js_import(js, (uintptr_t) print, "vi"));
    js_set(js, js_glob(js), "toggle_led", js_import(js, (uintptr_t) toggle_led, "v"));
    js_set(js, js_glob(js), "set_led", js_import(js, (uintptr_t) set_led, "vb"));
    js_set(js, js_glob(js), "delay", js_import(js, (uintptr_t) delay, "vi"));
}