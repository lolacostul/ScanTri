#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <SoftwareSerial.h>

SoftwareSerial waveshareSerial(0, 1); // RX, TX

#define BAR_CODE_SIZE 13    // 13 numbers

const byte scanningCommand[] = {0x7E, 0x00, 0x08, 0x01, 0x00, 0x02, 0x01, 0xAB, 0xCD };   // scanning command
/* Right after sending scanningCommand to barcode, it will respond with
0x02000001003333 in bytes and will begin scanning at the same time. */
int readBarCode = 0;
int resultBarCode[4*BAR_CODE_SIZE];
int * ptr = resultBarCode;
int firstCo = 1;
int cpt = 0;


void displayResult(int * tab) {
    Serial.println("\n\n---Display---");
    for (unsigned int i = 0; i < sizeof(BAR_CODE_SIZE)*4; i++) {
    // for (unsigned int i = 0; i < sizeof(tab)*4; i++) {
        Serial.print(tab[i]);
    }
    Serial.println("\n");
}

void resetTab(int * tab) {
    Serial.println("Reset");
    for (unsigned int i = 0; i < sizeof(BAR_CODE_SIZE)*4; i++) {
        tab[i] = 0;
    }
}

void printByte(byte b) {
    if (b < 0x10) 
        Serial.write('0');
    Serial.print(b, HEX);
    Serial.write('\t');
}


void setup() {
    Serial.begin(115200);
    waveshareSerial.begin(115200); // Waveshare Barcode Scanner
    Serial.println("Send scanning command to barcode");
    waveshareSerial.write(scanningCommand, sizeof(scanningCommand));
}

void loop() {
    if (waveshareSerial.available() > 0) {  // if barcode send data
        if (firstCo) {
            cpt++;
        }
    // if (readBarCode != -1) {  // if barcode send data
        readBarCode = waveshareSerial.read();
        printByte(readBarCode);

        if (isalnum(readBarCode)) {
            Serial.println((char) readBarCode);
            char code = (char)readBarCode;
            (*ptr) = (int)code % 48;
            ptr++;
            if (cpt == 7) {
                firstCo = 0;
                resetTab(resultBarCode);
                cpt = 0;
                ptr = resultBarCode;
            }
        } else {
            Serial.println(F("--"));
            displayResult(resultBarCode);
            Serial.println("Send barcode to database");
            resetTab(resultBarCode);
            ptr = resultBarCode;  // reset ptr addr
        }
    }
}
