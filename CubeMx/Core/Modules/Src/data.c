#include "main.h"
#include "stateMachine.h"
#include "stm32f4xx_hal_tim.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "data.h"
#include "stdbool.h"
#include "globals.h"

//#include "stepper.h"
//#include "motion.h"

Shape_t milling_queue[MAX_SHAPES];          //* Array in dem die Fräsdaten gespeichert werden
uint16_t shape_index = 0;                   //* Wie viele Formen aktuell


uint8_t event = 0;      //? temporäre variable, wird später nicht mehr benötigt


char temp_buffer[128];
uint8_t temp_index = 0;

float bewegung = 0;         //noch global, später lokal
char Modus;
float Drehzahl = 0;      //* Umdrehungen pro Minute
float Vorschub = 0;

uint16_t number_of_shapes = 0;      

//DataState_t current_data_state = CONFIG;

char line_buffer[128];      //Input buffer
uint8_t line_index = 0;     //position in string

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
                    //Stepper_StopAll();
                    send_ack();
                    event = 3;
                    millingMachine.state = INITIAL;                    
                }

                else if (millingMachine.state == INITIAL){      //§ INITIAL Modus
                    if (sscanf(temp_buffer, "e11,%c", &Modus) == 1){
                        event = 11;
                        //send_test(event);       //?  temporär
                        if (Modus == 'b'){
                            millingMachine.state = SET_X;
                        }
                        else if (Modus == 'f'){
                            millingMachine.state = SET_X;
                        }
                        else {
                            send_nack();
                        }
                    }
                }
                else if (millingMachine.state == CONFIG){       //§ CONFIG Modus
                    sscanf(temp_buffer, "e10,%f", &bewegung);

                    if (strcmp(temp_buffer, "e9") == 0){
                        //todo z-Wert speichern
                        send_ack();
                    }
                    else if(sscanf(temp_buffer, "e8,%f,%f", &Drehzahl, &Vorschub) == 2){
                        send_ack();                 
                    }

                    else if (sscanf(temp_buffer, "e7,%hu", &number_of_shapes) == 1){
                        if (number_of_shapes <= MAX_SHAPES){
                            send_ack();
                            millingMachine.state = TRANSFER;
                        }
                        else{
                            send_nack();
                        }
                    }
                    else{
                        send_nack();
                    }
                }

                else if (millingMachine.state == TRANSFER){            //§ Transfer Modus
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
                            millingMachine.state = READY;
                            send_ack();
                        }
                        send_next();
                       }
                       else {
                        send_nack();
                       }
                    }
                }
                else if (millingMachine.state == READY){             //§ Ready Modus
                    if (strcmp(temp_buffer, "e6") == 0){ 
                        millingMachine.state = MILLING;
                        send_ack();
                    }
                    else {
                        send_nack();
                    }
                }
                else if (millingMachine.state == MILLING){            //§ Milling Modus
                    if (strcmp(temp_buffer, "e4") == 0){
                        millingMachine.state = READY;
                        send_ack();
                    }
                    else{
                        send_nack();
                    }
                }
                temp_index = 0;     //Puffer für nächsten Befehl leeren
            }
        }
        else if (temp_index < sizeof(temp_buffer) -1){
            temp_buffer[temp_index++] = c;      //Zeichen sammeln
        }
    }
}