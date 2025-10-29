# Temperature-Responsive Light (Arduino WiFi MKR1010)

> **Ziyi Wang – CASA0014 Coursework Project (Oct 2025)**  
> A simple interactive IoT light system using Arduino WiFi MKR1010, temperature sensor and touch sensor.

---

## Project Overview

This project produced a device that can change the colours of a light based on temperature and compare the results of two detections.

- Built with **Arduino WiFi MKR1010**
- Uses **WiFi** + **MQTT** to communicate with the *Vespera* light system
- Integrates **a temperature sensor (MCP9700AE)** and **a touch sensor (HW-139)**
- Sends real-time color data to a remote LED array

---

## Workflow

<table>
  <tr>
    <td align="center"><img src="img/fig1.jpg" width="200"></td>
    <td align="center"><img src="img/fig2.jpg" width="200"></td>
    <td align="center"><img src="img/fig3.jpg" width="200"></td>
  </tr>
  <tr>
    <td align="center">1. Initial state (mode 0): left half lights on, right half lights off, waiting for the first detection.</td>
    <td align="center">2. Hold the temperature sensor, the colour of left half lights change with temperature, the higher the temperature, the more red appears.</td>
    <td align="center">3. Half fixed (mode 1): touch the touch sensor, left half lights fixed, active right half lights, waiting for the second detection.</td>
  </tr>
  <tr>
    <td align="center"><img src="img/fig4.jpg" width="200"></td>
    <td align="center"><img src="img/fig5.jpg" width="200"></td>
    <td align="center"><img src="img/fig6.jpg" width="200"></td>
  </tr>
  <tr>
    <td align="center">4. Hold the temperature sensor, the colour of right half lights change with temperature.</td>
    <td align="center">5. Compare state (mode 2): touch the touch sensor, both halves fixed, the temperatures of two detections can be compared.</td>
    <td align="center">6. Reset (back to mode 0): touch the touch sensor, reset to initial state.</td>
  </tr>
</table>


------

## Hardware Components

| Component | Description | Pin |
|------------|--------------|-----|
| **Arduino WiFi MKR1010** | Main microcontroller with WiFi | — |
| **Temperature Sensor (MCP9700AE)** | Outputs voltage proportional to temperature | A0 |
| **Touch Sensor (HW-139)** | Used to switch lighting modes | A2 |
| **Vespera / NeoPixel Light** | 72 LEDs receiving RGB values | via MQTT |

---

## Circuit Wiring

- **Temperature sensor (MCP9700AE)**
  
  | **VCC** | **3.3V** |
  | ------- | -------- |
  | **GND** | **GND**  |
  | **OUT** | **A0**   |
  
- **Touch sensor (HW-139)**
  
  | **VCC** | **3.3V** |
  | ------- | -------- |
  | **GND** | **GND**  |
  | **OUT** | **A2**   |

---

## Software Setup

1. **Install the following Arduino libraries:**
   
   - WiFiNINA
   - PubSubClient
   
2. **MQTT Configuration:**
   
   ```cpp
   const char* ssid = "CE-Hub-Student";
   const char* password = "...";
   const char* mqtt_server = "mqtt.cetools.org";
   const int mqtt_port = 1884;
   const char* mqtt_username = "student";
   const char* mqtt_password = "ce2021-mqtt-forget-whale";
   String lightId = "27";
   ```
   
3. **Upload the code (`v1.ino`)** to Arduino WiFi MKR1010.

---

## Light Modes

| Touch Count | Mode | Description |
|--------------|------|-------------|
| 0 | **Half active** | First half lights respond to temperature; second half off |
| 1 | **Half-fixed mode** | First half fixed color; second half lights up and shows current temperature |
| 2 | **Compare mode** | Both halves fixed colors and compare |
| 3 | **Reset** | Back to Mode 0 |

---

## Temperature → Color Mapping

| Temperature (°C) | LED Color |
|------------------|------------|
| 20°C | Blue |
| 27°C | Purple |
| 35°C | Red |

---

## Version History

| Version | Date | Description |
|----------|------|-------------|
| **v1** | Oct 2025 | Basic temperature-based lighting (no touch sensor) |
| **v2** | Oct 2025 | Added HW-139 touch sensor as ON/OFF switch |
| **v3** | Oct 2025 | Added temperature comparison mode (two-part lighting) |

---

## Enclosure & Physical Design

- **Controller housing:** paper box  (holds Arduino and touch sensor)
- **Temperature sensor:** placed outside box.

---

## Circuit Layout

<img width="1096" height="593" alt="circuit" src="https://github.com/user-attachments/assets/bc75a6df-fd60-4cb4-b8d7-59fb324fdbe5" />

---

## Product Appearance

![img1](https://github.com/user-attachments/assets/7ba59da9-f518-4905-a582-54d0ce1b1f69)
![img2](https://github.com/user-attachments/assets/cd011a0f-1f85-465c-840d-55d5cc9c5b8c)

---

## Author

**Ziyi Wang**  
University College London – CASA0014  
October 2025
