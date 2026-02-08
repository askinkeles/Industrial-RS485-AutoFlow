/*
 * Industrial-RS485-AutoFlow: Arduino Uno/Nano Example
 * Hardware-Managed Flow Control (Zero-Code)
 * * Bu kod, donanımsal akış denetimi sayesinde DE/RE pini yönetimi 
 * gerektirmeden RS485 üzerinden veri alışverişi yapar.
 */

void setup() {
  // RS485 modülü için standart seri portu başlatıyoruz.
  // Donanımsal çözümümüz sayesinde ekstra yön kontrol pini tanımlamıyoruz.
  Serial.begin(9600); // Endüstriyel standart baud hızı
  
  while (!Serial) {
    ; // Seri portun bağlanmasını bekle
  }
}

void loop() {
  // VERİ GÖNDERİMİ:
  // Sadece Serial.print kullanıyoruz. BC817 transistörümüz 
  // start bitini algıladığı anda yönü 'Transmit' moduna geçirecektir.
  Serial.println("ARDUINO_NODE_01: Donanımsal Akış Kontrolü Aktif.");
  
  // Veri gönderildikten sonra flush() çağrısı yapmaya veya 
  // bekleme (delay) eklemeye gerek yoktur; yön değişimi otomatik gerçekleşir.
  
  // VERİ OKUMA:
  // Modül boşta kaldığında otomatik olarak 'Receive' modundadır.
  if (Serial.available() > 0) {
    String incomingData = Serial.readStringUntil('\n');
    // Gelen veriyi işleyebiliriz
  }

  delay(1000); // 1 saniye bekleme
}