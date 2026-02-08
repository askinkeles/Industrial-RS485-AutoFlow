# 🏗️ Industrial RS485 Auto-Flow Communication Module

![Language](https://img.shields.io/badge/Language-TR%20%7C%20EN-blue)
![Platform](https://img.shields.io/badge/Platform-ESP32%20%7C%20Arduino-orange)
![License](https://img.shields.io/badge/License-MIT-green)

This project is a hardware-managed RS485 transceiver module that eliminates the need for software-based flow control (DE/RE pin management). It features automatic direction sensing and is fully ruggedized for industrial environments with high electrical noise and surge risks.

![Banner](/Assets/banner.png)

---

## 🛠️ Design Philosophy & Altium Standards
This module has been engineered using **Altium Designer** with a professional-grade approach.
* **Manufacturing Ready**: All fabrication files, including Gerbers, NC Drill files, and the Bill of Materials (BOM), have been generated to industry standards.
* **Signal Integrity**: The A and B differential pairs are routed symmetrically to minimize reflections and EMI, with a trace width of **20 mil (0.508 mm)** for enhanced industrial durability.

---

## 🔬 Technical Analysis & Engineering Rationale

### 1. Hardware-Managed Flow Control (Auto DE/RE)
The direction management is handled autonomously by a **BC817 (NPN)** transistor circuit.
* **Operation Logic**: When the UART line is idle (TX=High), the transistor is ON, pulling the DE/RE pins to GND and keeping the module in "Receive" mode. As soon as a Start Bit (TX=Low) is detected, the transistor turns OFF, and the 4.7kΩ pull-up resistor drives the DE/RE pins High, switching the module to "Transmit" mode.
* **Why No Series Resistor on TX?**: The MCU's TX pin drives both the DI input of the transceiver and the base of the BC817. Since the **4.7kΩ** base resistor already provides electrical isolation and current limiting, an additional series resistor was omitted to maintain signal integrity and sharp edge transitions.

### 2. Protection Layers & Component Selection
Surge transients and short circuits are the primary failure points in field deployments.

![Protection Circuit](/Assets/protection_logic.png)


* **Why the 0ZCJ0010FF2E PTC Fuse?**
  - **Voltage Rating ($V_{max}=60V$)**: Industrial systems typically operate at 24V. This PTC can withstand up to 60V DC, protecting the circuit even during severe wiring errors (e.g., accidental contact with the 24V power rail).
  - **Current Characteristics ($I_{hold}=100mA$ / $I_{trip}=300mA$)**: It maintains conduction without heating up to 100mA. In a short-circuit event where current hits 300mA, the resistance increases exponentially to enter "Safe Mode." The 1206 package was chosen for the best balance between size and thermal stability.
* **SM712 TVS Diode**: Selected specifically for the asymmetric bus voltage of RS485 ($-7V$ to $+12V$). Positioned after the PTC to shunt transient spikes to Ground.
* **RX Protection (470Ω)**: A series resistor is placed between the RO (Receiver Output) and the MCU's RX pin to limit current in case of latch-up or internal chip failure.

### 3. Bus Stability: Biasing & Termination
* **680Ω Bias Resistors**: These ensure that when the bus is idle, the differential voltage between A and B remains above the $200mV$ threshold, preventing "ghost characters" caused by noise.
* **120Ω Termination**: Activated via jumper. It must be enabled only at the two physical ends of the bus to suppress signal reflections.

---

## 🔌 Wiring & Application Guide

![Wiring Diagram](/Assets/wiring_diag.png)


| Module Pin | Connection | Note |
| :--- | :--- | :--- |
| **VCC** | 3.3V | DO NOT apply 5V. |
| **TX** | MCU TX | Triggers hardware flow control. |
| **RX** | MCU RX | 470Ω current-limited output. |
| **GND** | Common GND | Signal reference. |

---

## 💻 Software & Protocol Layer

### Zero-Code Advantage
No special libraries are required to manage direction pins (DE/RE). The hardware handles the physical layer transition automatically.

```cpp
void setup() {
  // No extra pin definition needed for DE/RE.
  Serial2.begin(115200, SERIAL_8N1, 16, 17); 
}

void loop() {
  Serial2.println("INDUSTRIAL-RS485-READY");
  delay(1000);
}
```

### Modbus RTU & Timing Precision
In packet-based protocols like Modbus RTU, the transition from "Transmit" to "Receive" is critical.
* **Software Latency**: In software-controlled setups, delays during `Serial.flush()` or manual pin toggling can cause the first few bits of a response to be missed.
* **Hardware Solution**: This module switches direction at hardware speed, triggered by the last bit of the UART frame. This eliminates timing jitter and allows "Manual DE/RE" management to be completely disabled in Modbus libraries.

![Signal Analysis](/Assets/scope_capture.png)

---

## 📚 Related Guides (References)
For more in-depth theoretical details and field experience, check out my other repositories:
* [Industrial Serial Communications Guide](https://github.com/askinkeles/Industrial-Serial-Comms-Guide)
* [Embedded UART Guide](https://github.com/askinkeles/Embedded-UART-Guide)

---

## 📊 Bill of Materials (BOM)

| Component | Value / Code | Package | Technical Rationale |
| :--- | :--- | :--- | :--- |
| **U1** | MAX3485 / SP3485 | SOIC-8 | 3.3V Low Power Consumption |
| **Q1** | BC817 (NPN) | SOT-23 | High-Speed Switching Performance |
| **D1** | SM712 | SOT-23 | Asymmetric Bus Protection (-7V/+12V) |
| **F1, F2** | 0ZCJ0010FF2E | 1206 | 60V / 100mA Short Circuit Protection |
| **R_Bias** | 680Ω | 0805 | Optimized Bias for 115.2kbps |
| **R_Flow** | 4.7kΩ | 0805 | Minimized DE/RE Propagation Delay |
