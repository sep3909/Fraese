#include "main.h"
#include "stm32f4xx_hal_tim.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "data.h"
#include "stdbool.h"
#include "stepper.h"
#include "motion.h"

#define MAX_SHAPES 100

//* STATE_MACHINE Daten empfangen
typedef enum{
    CONFIG,
    TRANSFER,
    MILLING,
    READY,
}DataState_t;

//* Formen
typedef enum{
    KREIS,
    RECHTECK,
    DREIECK,
    LINIE,
}Form_t;

typedef struct{
    Form_t type;           //Was für eine Form
    float t;            //Tiefe für alle Formen relevant
    union {
        struct {
            float x;
            float y;
            float r;
        }kreis;
        struct{
            float x1;
            float y1;
            float x2;
            float y2;
        }rechteck;
        struct{
            float x1;
            float y1;
            float x2;
            float y2;
            float x3;
            float y3;
        }dreieck;
        struct {
            float x1;
            float y1;
            float x2;
            float y2;
        }linie;
    }geo;               //geometrische Daten
}Shape_t;

Shape_t milling_queue[MAX_SHAPES];          //* Array in dem die Fräsdaten gespeichert werden
uint16_t shape_index = 0;                   //* Wie viele Formen aktuell

extern float pos_x;
extern float pos_y;
extern float pos_z;

uint16_t Drehzahl = 0;      //* Umdrehungen pro Minute
uint16_t Vorschub = 0;
uint8_t event = 0;      //? temporäre variable, wird später nicht mehr benötigt


char temp_buffer[128];
uint8_t temp_index = 0;

char Modus;
float bewegung = 0;         //noch global, später lokal

uint16_t number_of_shapes = 0;      

DataState_t current_data_state = CONFIG;

char line_buffer[128];      //Input buffer
uint8_t line_index = 0;     //position in string

//* Timer Interrupt Aufruf alle 200ms
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if (htim->Instance == TIM3){
        if (current_data_state == MILLING){                         //* nur wenn gerade gefräst wird, werden Daten gesendet
            send_position(pos_x, pos_y, pos_z);
        }         
    }
  }

//? function for sending i.O.
void send_ack(){
    char *reply = "e0,ack\n";
    CDC_Transmit_FS((uint8_t*)reply, strlen(reply));
}

//? function for sending n.i.O.
void send_nack(){
    char *reply = "e0,nack\n";
    CDC_Transmit_FS((uint8_t*)reply, strlen(reply));
}

//? function for sending next
void send_next(){
    char *reply = "e0,next\n";
    CDC_Transmit_FS((uint8_t*)reply, strlen(reply));
}

//?function for sending current position
void send_position(float x, float y, float z){
    char static reply[64];
    sprintf(reply, "e5,%.2f,%.2f,%.2f\n", x,y,z);          //Converting Numbers to String
    CDC_Transmit_FS((uint8_t*)reply, strlen(reply));
}

//? sendet Not-Stopp Nachricht
void send_emergency_stopp(){
    char *reply = "e1\n";
    CDC_Transmit_FS((uint8_t*)reply, strlen(reply));
}

//? temporäre Testfunktion
void send_test(int i){
    char static reply [64];
    if (i == 8){
        sprintf(reply, "%hu, %hu\n", Drehzahl, Vorschub);
        CDC_Transmit_FS((uint8_t*)reply, strlen(reply));
    }
    else if (i == 10){
        sprintf(reply, "%.2f\n", bewegung);
        CDC_Transmit_FS((uint8_t*)reply, strlen(reply));
    }
    else{
        sprintf(reply, "%d\n", i);
        CDC_Transmit_FS((uint8_t*)reply, strlen(reply));
    }
}


//? function for reading event
void read_data(uint8_t* buf, uint32_t len){
    //float bewegung = 0; //z-bewegung bei config   später lokal, aktuell global
    for (uint32_t i = 0; i < len; i++){
        char c = (char)buf[i];
        if (c == '\n' || c == '\r'){        // Wenn Zeilenende erkannt
            if (temp_index > 0) // Nur kopieren, wenn wir auch Daten haben
            {
                temp_buffer[temp_index] = '\0'; // String Ende setzen

                //! Not-Stopp immer aktiv
                if (strcmp(temp_buffer, "e3") == 0){
                    Stepper_StopAll();
                    event = 3;                    //? temporär
                    send_test(event);          //? temporär
                }

                else if (current_data_state == CONFIG){      //§ Config Modus
                    if (sscanf(temp_buffer, "e11,%c", &Modus) == 1){
                        event = 11;
                        send_test(event);       //?  temporär
                        if (Modus == 'b'){
                            //todo State auf bohren setzen
                            send_ack();
                        }
                        else if (Modus == 'f'){
                            //todo State auf fräsen setzen
                            send_ack();
                        }
                        else {
                            send_nack();
                        }
                    }
                    else if (sscanf(temp_buffer, "e10,%f", &bewegung) == 1){
                        event = 10;             //?temporär
                        send_test(event);       //?temporär
                        //todo z ansteuern
                    }
                    else if (strcmp(temp_buffer, "e9") == 0){
                        //todo z-Wert speichern
                    }
                    else if(sscanf(temp_buffer, "e8,%hu,%hu", &Drehzahl, &Vorschub) == 2){
                        event = 8;                  //?temporär
                        send_test(event);       //?temporär
                    }
                    else if (sscanf(temp_buffer, "e7,%hu", &number_of_shapes) == 1){
                        if (number_of_shapes <= MAX_SHAPES){
                            send_ack();
                            current_data_state = TRANSFER;
                        }
                        else{
                            send_nack();
                        }
                    }
                    else{
                        send_nack();
                    }
                }

                else if (current_data_state == TRANSFER){            //§ Transfer Modus
                    float x1, y1, x2, y2, x3, y3, r, t;         //Temporäre Variablen zum parsen
                    bool parsed_successfully = false;
                    if (shape_index < number_of_shapes){
                       //* Kreis
                       if (sscanf(temp_buffer, "kreis,%f,%f,%f,%f", &x1, &y2, &r, &t) == 4){
                        milling_queue[shape_index].type = KREIS;
                        milling_queue[shape_index].t = t;
                        milling_queue[shape_index].geo.kreis.x = x1;
                        milling_queue[shape_index].geo.kreis.y = y1;
                        milling_queue[shape_index].geo.kreis.r = r;
                        parsed_successfully = true;
                       } 

                       //* Rechteck
                       else if (sscanf(temp_buffer, "rechteck, %f,%f,%f,%f,%f", &x1, &y1, &x2, &y2, &t) == 5){
                        milling_queue[shape_index].type = RECHTECK;
                        milling_queue[shape_index].t = t;
                        milling_queue[shape_index].geo.rechteck.x1 = x1;
                        milling_queue[shape_index].geo.rechteck.y1 = y1;
                        milling_queue[shape_index].geo.rechteck.x2 = x2;
                        milling_queue[shape_index].geo.rechteck.y2 = y2;
                        parsed_successfully = true;
                       }

                       //* Dreieck
                       else if(sscanf(temp_buffer, "dreieck,%f,%f,%f,%f,%f,%f,%f", &t, &x1, &y1, &x2, &y2, &x3, &y3) == 7){
                        milling_queue[shape_index].type = DREIECK;
                        milling_queue[shape_index].t = t;
                        milling_queue[shape_index].geo.dreieck.x1 = x1;
                        milling_queue[shape_index].geo.dreieck.y1 = y1;
                        milling_queue[shape_index].geo.dreieck.x2 = x2;
                        milling_queue[shape_index].geo.dreieck.y1 = y2;
                        milling_queue[shape_index].geo.dreieck.x3 = x3;
                        milling_queue[shape_index].geo.dreieck.y3 = y3;
                        parsed_successfully = true;
                       }

                       //* Linie
                       else if (sscanf(temp_buffer, "linie, %f,%f,%f,%f,%f", &x1, &y1, &x2, &y2, &t) == 5){
                        milling_queue[shape_index].type = LINIE;
                        milling_queue[shape_index].t = t;
                        milling_queue[shape_index].geo.linie.x1 = x1;
                        milling_queue[shape_index].geo.linie.y1 = y1;
                        milling_queue[shape_index].geo.linie.x2 = x2;
                        milling_queue[shape_index].geo.linie.y2 = y2;
                        parsed_successfully = true;
                       }
                       if (parsed_successfully == true){
                        shape_index ++;     //Nächste Form
                        if (shape_index >= number_of_shapes){
                            current_data_state = READY;
                            send_ack();
                        }
                        send_next();
                       }
                       else {
                        send_nack();
                       }
                    }
                }
                else if (current_data_state == READY){             //§ Ready Modus
                    if (strcmp(temp_buffer, "e6") == 0){ 
                        current_data_state == MILLING;       
                        //todo motor start
                    }
                    else {
                        send_nack();
                    }
                }
                else if (current_data_state == MILLING){            //§ Milling Modus
                    if (strcmp(temp_buffer, "e4")){
                        current_data_state == READY;
                        //todo motor stop
                    }
                    else{
                        send_nack();
                    }
                }

                //todo play und milling
                temp_index = 0;     //Puffer für nächsten Befehl leeren
            }
        }
        else if (temp_index < sizeof(temp_buffer) -1){
            temp_buffer[temp_index++] = c;      //Zeichen sammeln
        }
    }
}