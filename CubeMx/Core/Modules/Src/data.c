#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include <stdint.h>
#include <stdio.h>
#include "data.h"

#define MAX_SHAPES = 500;

//? Formen
typedef enum{
    KREIS,
    RECHTECK,
    DREIECK,
    LINIE,
}Form;

typedef struct{
    float x, y, r, t;
}Kreis;


extern float pos_x;
extern float pos_y;
extern float pos_z;
uint16_t Drehzahl = 0;      //* Umdrehungen pro Minute
uint16_t Vorschub = 0;
uint8_t event = 0;
uint8_t DataReady = 0;
extern uint8_t  UserRxBuffer[128];

char line_buffer[128];      //Input buffer
uint8_t line_index = 0;     //position in string

//* Timer Interrupt Aufruf alle 200ms
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if (htim->Instance == TIM3){
      read_event(event);            
    }
  }

//? function for sending i.O.
void send_ack(){
    char *reply = "e0,ack\n";
    CDC_Transmit_FS((uint8_t*)reply, strlen(reply));
}

//? temporäre Testfunktion
void send_test(int i){
    char static reply [64];
    if (i == 8){
        sprintf(reply, "%hu, %hu\n", Vorschub, Drehzahl);
        CDC_Transmit_FS((uint8_t*)reply, strlen(reply));
    }
    else{
        sprintf(reply, "%d\n", i);
        CDC_Transmit_FS((uint8_t*)reply, strlen(reply));
    }
}

//? function for sending n.i.O.
void send_nack(){
    char *reply = "e0,nack\n";
    CDC_Transmit_FS((uint8_t*)reply, strlen(reply));
}

//?function for sending current position
void send_position(float x, float y, float z){
    char static reply[64];
    sprintf(reply, "%.2f, %.2f, %.2f\n", x,y,z);          //Converting Numbers to String
    CDC_Transmit_FS((uint8_t*)reply, strlen(reply));
}

//? function for reading event
void read_event(){
    if (DataReady == 1){
        DataReady = 0;      //DataReady zurücksetzen für neues lesen
        if (strcmp((char*)UserRxBuffer, "e3") == 0){
            event = 3;
            send_test(event);
        }
        else if (strcmp((char*)UserRxBuffer, "e4") == 0){
            event = 4;
            send_test(event);
        }
        else if (strcmp((char*)UserRxBuffer, "e6") == 0){
            event = 6;
            send_test(event);
        }
        else if (strcmp((char*)UserRxBuffer, "e7,s") == 0){
            event = 7;
            send_test(event);
        }
        else if (sscanf((char*)UserRxBuffer, "e8,%hu,%hu", &Drehzahl, &Vorschub) == 2){
            event = 8;
            send_test(event);
        }
        else if (strcmp((char*)UserRxBuffer, "e9") == 0){
            event = 9;
            send_test(event);
        }
    }
}


