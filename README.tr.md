# 🏗️ Endüstriyel RS485 Otomatik Akış Denetimli İletişim Modülü

[![English](https://img.shields.io/badge/Language-EN-red)](README.md)
[![Platform](https://img.shields.io/badge/Platform-ESP32%20%7C%20Arduino-orange)](#)
[![License](https://img.shields.io/badge/License-MIT-green)](#)

Bu proje; mikroişlemci tarafında herhangi bir yazılımsal akış denetimi (DE/RE pini yönetimi) gerektirmeyen, donanımsal olarak otomatik yön tayini yapan ve endüstriyel sahalardaki zorlu elektriksel koşullara karşı tam zırhlandırılmış bir RS485 alıcı-verici modülüdür.

![Banner](/Assets/banner.png)

---

## 🛠️ Tasarım Felsefesi ve Altium Standartları
Bu modül, endüstri standardı olan **Altium Designer** kullanılarak profesyonel bir yaklaşımla tasarlanmıştır. 
* **Üretim Hazırlığı**: Tüm üretim (Gerber), delik delme (NC Drill) ve malzeme listesi (BOM) dosyaları Altium üzerinden üretim bandına uygun standartlarda oluşturulmuştur.
* **Sinyal Bütünlüğü**: A ve B diferansiyel yolları, yansımayı ve gürültüyü en aza indirecek şekilde simetrik ve endüstriyel dayanım için **20 mil (0.508 mm)** kalınlığında çizilmiştir.

---

## 🔬 Teknik Analiz ve Mühendislik Kararları

### 1. Donanımsal Akış Denetimi (Auto Flow Control)
Yazılım yükünü ortadan kaldıran bu yapı, **BC817 (NPN)** transistör üzerinden kurgulanmıştır.
* **Çalışma Mantığı**: UART hattı boşta (TX=Yüksek) iken transistör iletime geçer (ON) ve DE/RE uçlarını şaseye çekerek modülü "Dinleme" modunda tutar. Start biti (TX=Düşük) geldiği anda transistör kesime gider (OFF) ve 4.7kΩ yukarı çekme direnci üzerinden DE/RE uçları "Yüksek" seviyeye çıkarak modülü "Gönderme" moduna alır.
* **Neden TX Hattında Seri Direnç Yok?**: İşlemcinin TX pini hem MAX3485'in DI girişini hem de BC817'nin beyz direnç hattını sürer. Transistörün beyz ucundaki **4.7kΩ** direnç zaten elektriksel bir izolasyon ve akım sınırlaması sağladığı için, TX hattında ekstra bir seri direnç kullanılarak sinyal bütünlüğü riske atılmamıştır.

### 2. Koruma Katmanları ve Bileşen Seçim Gerekçeleri

![Koruma Devresi](/Assets/protection_logic.png)

* **Neden PTC Sigorta (0ZCJ0010FF2E) Seçildi?**
  - **Voltaj Dayanımı ($V_{max}=60V$)**: Endüstriyel sistemler genellikle 24V ile çalışır. Bu PTC, 60V DC dayanımı sayesinde hatalı kablolama durumunda bile (hattın yanlışlıkla 24V besleme hattına değmesi vb.) devreyi koruyabilir.
  - **Akım Karakteristiği ($I_{hold}=100mA$ / $I_{trip}=300mA$)**: 100mA'e kadar ısınmadan iletimde kalır. Kısa devre anında direnci hızla artarak devreyi güvenli moda sokar. 1206 kılıf seçilerek küçük boyut ile termal kararlılık dengelenmiştir.
* **SM712 TVS Diyot**: RS485 hattının asimetrik yapısına ($-7V$ / $+12V$) uygun tek profesyonel çözümdür. PTC'den sonra konumlandırılarak ani voltaj yükselmelerini şaseye boşaltır.
* **RX Koruma (470Ω)**: RO (Receiver Output) ucundaki voltaj sıçramalarına karşı mikroişlemcinin RX pinini korumak amacıyla akım sınırlayıcı olarak eklenmiştir.

### 3. Hat Kararlılığı: Bias ve Sonlandırma
* **680Ω Bias Dirençleri**: Hattın boşta (Idle) kaldığı anlarda A ve B arasındaki farkın $200mV$ eşiğinin altına düşmesini engelleyerek gürültü kaynaklı "hayalet veri" oluşumunu önler.
* **120Ω Sonlandırma**: Jumper aracılığıyla devreye alınır. Sinyal yansımasını sönümlemek için hattın sadece fiziksel iki ucunda aktif edilmelidir.

---

## 🔌 Bağlantı Rehberi ve Uygulama

![Bağlantı Şeması](/Assets/wiring_diag.png)

| Modül Pin | Bağlantı | Not |
| :--- | :--- | :--- |
| **VCC** | 3.3V | Kesinlikle 5V uygulanmamalıdır. |
| **TX** | MCU TX | Donanımsal yön denetimi tetikler. |
| **RX** | MCU RX | 470Ω akım sınırlamalı çıkış. |
| **GND** | Ortak Şase | Haberleşme referansı. |

---

## 💻 Yazılım ve Protokol Katmanı

### Sıfır-Kod (Zero-Code) Avantajı
Bu modül ile haberleşmek için yön yönetimi (DE/RE) yapan özel bir kütüphaneye gerek yoktur. Donanım, fiziksel katmandaki yön değişimini otomatik halleder.

```cpp
void setup() {
  // DE/RE için ekstra bir pin tanımlamaya gerek yoktur.
  Serial2.begin(115200, SERIAL_8N1, 16, 17); 
}

void loop() {
  Serial2.println("INDUSTRIAL-RS485-READY");
  delay(1000);
}
```

### Modbus RTU ve Zamanlama (Timing) Derinliği
Modbus RTU paket tabanlı protokollerde en kritik nokta, veri gönderimi biter bitmez hattın "Dinleme" moduna çekilmesidir. 
* **Yazılımsal Gecikme**: Yazılımsal kontrolde Serial.flush() sonrası DE/RE pini çekilirken oluşan mikro saniyeler düzeyindeki gecikmeler, karşı cihazın cevabının ilk bitlerinin kaçırılmasına neden olabilir.
* **Donanımsal Çözüm**: Bu kartta yön değişimi, UART hattındaki TX sinyalinin son bitiyle eş zamanlı olarak donanımsal hızda gerçekleşir. Bu durum, Modbus RTU haberleşmesinde zamanlama hatalarını sıfıra indirir ve "Manual DE/RE" yönetimine duyulan ihtiyacı ortadan kaldırır.

![Sinyal Analizi](/Assets/scope_capture.png)

---

## 📚 İlgili Rehberler (Referanslar)
Tasarımın teorik detayları ve saha tecrübeleri için diğer çalışmalarımı inceleyebilirsiniz:
* [Industrial Serial Communications Guide](https://github.com/askinkeles/Industrial-Serial-Comms-Guide)
* [Embedded UART Guide](https://github.com/askinkeles/Embedded-UART-Guide)

---

## 📊 Malzeme Listesi (BOM)

| Parça | Değer / Kod | Kılıf | Teknik Neden |
| :--- | :--- | :--- | :--- |
| **U1** | MAX3485 / SP3485 | SOIC-8 | 3.3V Düşük Güç Tüketimi |
| **Q1** | BC817 (NPN) | SOT-23 | Hızlı Anahtarlama Performansı |
| **D1** | SM712 | SOT-23 | Asimetrik Bus Koruması (-7V/+12V) |
| **F1, F2** | 0ZCJ0010FF2E | 1206 | 60V / 100mA Kısa Devre Koruması |
| **R_Bias** | 680Ω | 0805 | 115.2kbps için Optimize Edilmiş Bias |
| **R_Flow** | 4.7kΩ | 0805 | DE/RE Geçiş Gecikmesi Minimizasyonu |
