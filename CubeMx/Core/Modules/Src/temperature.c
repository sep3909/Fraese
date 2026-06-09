#include "adc.h"
#include "dma.h"
#include <stdint.h>
#include <math.h>
#include "stdbool.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "temperature.h"

#define T_N 300.0f
#define R_1 10000.0f
#define R_N 7000.0f
#define V_ref 3.2f
#define B 4000.0f


volatile uint16_t adc_values[4];    //$ alle vier Tempsensoren werden in dem Array gespeichert bzw. deren Werte
float v_ntc[4];                     //% Array mit allen Temp. Werten
bool Lüfter_an = false;

void temp_init(void){
    MX_DMA_Init();
    MX_ADC1_Init();
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, 4);
}

void temp_messung(void){

    float v_highest = 0;        //$ höchster Spannungswert
    

    v_ntc[0] = ((float)adc_values[0] * 3.3f)/4095.0f;       //% aus ADC-Werten Spannungen berechnen
    v_ntc[1] = ((float)adc_values[1] * 3.3f)/4095.0f;
    v_ntc[2] = ((float)adc_values[2] * 3.3f)/4095.0f;
    v_ntc[3] = ((float)adc_values[3] * 3.3f)/4095.0f;

    for (int i = 0; i <= 3; i++){
        if (v_ntc[i] >= v_highest){
            v_highest = v_ntc[i];
        }
    }

    float R_NTC = R_1 * (v_highest/(V_ref - v_highest));
    float temp = 1.0f / (((1.0f/T_N) + (1.0f/B) * logf(R_NTC/R_N)));

    if (temp >= 330 && Lüfter_an == false){
        Lüfter_an = true;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    }
    if (temp <= 300 && Lüfter_an == true){
        Lüfter_an = false;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    }
}