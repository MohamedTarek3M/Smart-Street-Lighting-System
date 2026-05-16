# 💡 WS2812 RGB LED Wiring & Configuration Guide

This document outlines the physical wiring, color coding, and best practices for the WS2812 RGB LED system used in the **Smart Street Lighting System**.

---

## 📌 Standard Pinout Configuration

Each WS2812 LED strip (or pillar) typically features a 4-pin connector. Here is the standard function for each pin:

| Pin | Function | Description |
| :--- | :--- | :--- |
| **Dout** | Data Out | Passes the control signal to the next chained LED strip. |
| **VDD** | Power (+5V) | Main power supply for the LEDs. |
| **GND** | Ground (-) | Common ground connection. |
| **Din** | Data In | Receives the control signal from the ESP32 or previous LED. |

---

## 🌈 Zone-Specific Wire Color Coding

To simplify installation and maintenance, the physical wires for each lighting zone are color-coded as follows:

### 🔹 Zone 1 (Pillars 1 & 2)

| Pin | Wire Color | Notes |
| :--- | :--- | :--- |
| **VDD** | 🔴 Red | Connect to 5V power supply |
| **GND** | ⚫ Black | Connect to common ground |
| **Din** | 🔵 Blue | Connect to ESP32 Data Pin |
| **Dout**| 🟠🟡 Orange/Yellow | Connect to next pillar's `Din` (if chaining) |

### 🔸 Zone 2 (Pillars 3 & 4)

| Pin | Wire Color | Notes |
| :--- | :--- | :--- |
| **VDD** | 🟢 Green | Connect to 5V power supply |
| **GND** | ⚫ Black | Connect to common ground |
| **Din** | 🟡 Yellow | Connect to ESP32 Data Pin |
| **Dout**| 🟣⚪ Purple/White | Connect to next pillar's `Din` (if chaining) |

---

## 🔌 ESP32 Integration

In the current **Smart Street Lighting System** firmware, each pillar uses an independent data pin for maximum reliability and control. 

| Zone | Pillar | ESP32 Pin |
| :--- | :--- | :--- |
| **Zone 1** | Pillar 1 | `GPIO 5` |
| **Zone 1** | Pillar 2 | `GPIO 19` |
| **Zone 2** | Pillar 3 | `GPIO 13` |
| **Zone 2** | Pillar 4 | `GPIO 14` |

> [!NOTE]
> If you decide to daisy-chain pillars together using the `Dout` wire instead of independent pins, you will need to update the `FastLED.addLeds` configuration in `Smart_Street_Lighting_System.ino` to reflect a single data pin per zone.

---

## ⚠️ Important Safety Notes

* **Check Voltage:** Ensure your power supply matches the WS2812 requirement exactly (typically **5V**). Supplying a higher voltage will permanently destroy the LEDs.
* **Common Ground:** The **GND** of your 5V power supply **must** be connected to the **GND** of the ESP32. Without a shared ground, the data signal will fail or glitch.
* **Directionality:** Data only flows in one direction: from **Din** to **Dout**. Reversing this will result in the LEDs not functioning.
* **Power Injection:** For long runs of LEDs, power (VDD and GND) should be injected at multiple points to prevent voltage drop and discoloration (yellowing of white light).