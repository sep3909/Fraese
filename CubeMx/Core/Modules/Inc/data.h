#ifndef DATA_H
#define DATA_H

// #include "main.h"
// #include "i2c.h"
// #include "i2s.h"
// #include "spi.h"
// #include "stm32f4xx_hal_tim.h"
// #include "tim.h"
// #include "usb_device.h"
// #include "gpio.h"
// #include "usbd_cdc_if.h"

#include <stdint.h>
#include <stddef.h>

//%------------------------------------------------------------------------------------------
//% LEGENDE
//% e0,ack: sendet der STM an die GUI um ein positive Bestätigung zu senden
//% e0,nack: sendet der STM an die GUI um eine negative Bestätigung zu senden
//% e0,next: sendet der STM an die GUI um zu signalisieren, dass die nächste Form gesendet werden kann
//% e1: emergency stop, sendet STM an GUI im Falle eines Not-Stopps
//% e2: fertig gefräst, sendet STM an GUI
//% e3: Stop-Befehl von GUI an STM
//% e4: Pause-Befehl von GUI an STM
//% e5,x,y,z: aktuelle Position wird an GUI gesendet
//% e6: play-Befehl von GUI an STM
//% e7,zahl: Start der Datenübertragung mit Anzahl an Formen wird übertragen von GUI zu STM
//% e8,zahl,zahl: Drehzahl und Vorschub wird von GUI an STM übertragen
//% e9: Nutzer legt z-Pos fest von GUI zu STM
//% e10,zahl: z-Pos Konfiguration von GUI zu STM
//% e11,f: Nutzer legt Modus auf fräsen fest von GUI zu STM
//% e11,b: Nutzer legt Modus auf bohren fest von GUI zu STM
//%-------------------------------------------------------------------------------------------

#define MAX_SHAPES 200


//* Formen
typedef enum{
    KREIS,
    RECHTECK,
    DREIECK,
    LINIE,
    PUNKT
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
        struct {
            float x1;
            float y1;
        }punkt;
    }geo;               //geometrische Daten
}Shape_t;

void send_ack();        //? Datenpaket i.O.     
void send_nack();       //? Datenpaket n.i.O.
void send_next();       //? nächste Form bitte

void send_position(float x, float y, float z);      //? aktuelle Position des Fräsers nur im Milling State aktiv, alle 200ms
void read_data(uint8_t* buf, uint32_t len);         //? liest Daten aus

void send_test(int i);                              //? temporäre Testfunktion
void send_emergency_stopp();                        //? sendet Not-Stopp

#endif /* DATA_H */
