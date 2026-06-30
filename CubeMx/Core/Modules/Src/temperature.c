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
#define R_N 10000.0f
#define V_ref 2.9f
#define B 4000.0f


volatile uint16_t adc_values[4];    //$ alle vier Tempsensoren werden in dem Array gespeichert bzw. deren Werte
float v_ntc_min;                     //% Array mit allen Temp. Werten
bool Lüfter_an = false;
float volatile temp = 0;

void temp_init(void){
    MX_DMA_Init();
    MX_ADC1_Init();
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, 4);
}

void temp_messung(void){

    uint16_t adcValueMin = 4096;        //$ höchster Spannungswert
    
    for (int i = 0; i <= 3; i++){
        if (adc_values[i] <= adcValueMin){
            adcValueMin = adc_values[i];
        }
    }

    v_ntc_min = ((float)adcValueMin * V_ref)/4095.0f;       //% aus ADC-Werten Spannungen berechnen

    float R_NTC = R_1 * (v_ntc_min/(V_ref - v_ntc_min));
    temp = 1.0f / (((1.0f/T_N) + (1.0f/B) * logf(R_NTC/R_N)));

    if (temp >= 340.0f && Lüfter_an == false){
        Lüfter_an = true;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    }
    if (temp <= 325.0f && Lüfter_an == true){
        Lüfter_an = false;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    }
}