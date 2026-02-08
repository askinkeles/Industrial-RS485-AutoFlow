/*
 * Industrial-RS485-AutoFlow: ESP32 DevKit Example
 * Developed for Solution Architects & Industrial Applications
 */

#include <Arduino.h>

// ESP32 UART2 Pin Tanımlamaları
#define RXD2 16
#define TXD2 17

void setup() {
  // Hata ayıklama için USB seri portu
  Serial.begin(115200);

  // RS485 Modülü için UART2 başlatılıyor.
  // Hardware-Managed mimari sayesinde DE/RE kontrol pinine GEREK YOKTUR.
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  
  Serial.println("ESP32 RS485 Modülü Hazır (Zero-Code Mode)");
}

void loop() {
  // Mesaj Gönderimi
  Serial2.println("ESP32_NODE_02: Veri donanımsal zamanlama ile gönderildi.");
  
  // Gelen Mesaj Kontrolü
  if (Serial2.available()) {
    char c = Serial2.read();
    Serial.print("RS485'ten Gelen: ");
    Serial.println(c);
  }

  delay(2000);
}