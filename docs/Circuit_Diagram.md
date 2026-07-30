# 🔌 Smart Street Lighting System - Circuit Diagram (No PIR)

This document provides the technical wiring and circuit architecture for the Smart Street Lighting System. As requested, the **PIR Motion Sensor has been excluded** from this configuration, relying on the **IR Obstacle Sensor** for localized traffic detection.

---

## 📐 Schematic Diagram (Mermaid)

```mermaid
graph TD
    %% Main Controller
    ESP32["ESP32 DevKit V1"]

    %% Power System
    subgraph "Power Bank"
        B1["ICR 18650 Cell"]
        B2["ICR 18650 Cell"]
        B3["ICR 18650 Cell"]
        B1 & B2 & B3 -->|"Parallel Connection (3.7V - 4.2V)"| BatBank["3x 18650 Battery Pack"]
    end

    BatBank -->|"VCC"| INA_IN["INA219 (V+ / Vin)"]
    INA_IN -->|"V- / Vout"| Buck["Buck Converter (XL4015)"]
    Buck -->|"5V & GND"| ESP32["ESP32 (on Breadboard)"]
    Buck -->|"5V & GND"| LEDs["WS2812B LEDs (Pillars 1-4)"]
    
    %% Power Monitoring Data
    ESP32 <-->|I2C: SDA 21, SCL 22| INA_DATA["INA219 Data Interface"]

    %% Input Sensors
    LDR["LDR (Light Sensor)"] -->|Analog: GPIO 34| ESP32
    IR["IR Obstacle Sensor"] -->|Digital: GPIO 26| ESP32
    Rain["Rain Sensor"] -->|Digital: GPIO 25| ESP32
    DHT["DHT22 (Climate)"] -->|Data: GPIO 4| ESP32

    %% Output Indicators
    ESP32 -->|Digital: GPIO 33| LightBox["Interior Light (Pin Inspection)"]
    LightBox ---|GND: GPIO 32| ESP32

    %% Lighting Zones
    subgraph "Lighting Zone 1"
        ESP32 -->|Data: GPIO 5| P1["Pillar 1 LEDs"]
        ESP32 -->|Data: GPIO 19| P2["Pillar 2 LEDs"]
    end

    subgraph "Lighting Zone 2"
        ESP32 -->|Data: GPIO 13| P3["Pillar 3 LEDs"]
        ESP32 -->|Data: GPIO 14| P4["Pillar 4 LEDs"]
    end

    %% Styles
    style ESP32 fill:#2c3e50,stroke:#34495e,stroke-width:2px,color:#fff
    style Battery fill:#f1c40f,stroke:#f39c12,stroke-width:2px
    style LEDs fill:#3498db,stroke:#2980b9,stroke-width:2px,color:#fff
    style LightBox fill:#e74c3c,stroke:#c0392b,stroke-width:2px,color:#fff
```

---

## 📌 Detailed Pinout Table

| Component | ESP32 Pin | Function | Notes |
| :--- | :--- | :--- | :--- |
| **LDR** | `GPIO 34` | Analog Input | Measures ambient light levels. |
| **IR Sensor** | `GPIO 26` | Digital Input | Detects traffic/obstacles (Active LOW). |
| **Rain Sensor** | `GPIO 25` | Digital Input | Detects precipitation (Active LOW). |
| **DHT22** | `GPIO 4` | Data | Climate monitoring (10k Pull-up recommended). |
| **INA219** | `GPIO 21 (SDA)` | I2C Data | Power/Battery monitoring. |
| **INA219** | `GPIO 22 (SCL)` | I2C Clock | Power/Battery monitoring. |
| **Pillar 1** | `GPIO 5` | WS2812B Data | Zone 1 illumination. |
| **Pillar 2** | `GPIO 19` | WS2812B Data | Zone 1 illumination. |
| **Pillar 3** | `GPIO 13` | WS2812B Data | Zone 2 illumination. |
| **Pillar 4** | `GPIO 14` | WS2812B Data | Zone 2 illumination. |
| **Light Box** | `GPIO 33` | LED Anode (+) | Interior light for ESP32 pin inspection. |
| **Light Box** | `GPIO 32` | LED Cathode (-) | Software-controlled Ground. |

---

## ⚡ Power Considerations

> [!WARNING]
> **Voltage Threshold Discrepancy:**
> You have requested a **parallel** connection for the three ICR 18650 cells. This results in a nominal voltage of **3.7V** (4.2V max). 
> However, the current firmware (`Smart_Street_Lighting_System.ino`) has the following safety thresholds:
> * `BATTERY_LOW = 10.5V`
> * `BATTERY_CRITICAL = 9.0V`
>
> With a parallel battery pack, the system will permanently trigger the **Critical Low Battery** state. To fix this, you should either:
> 1.  Wire the cells in **Series (3S)** to reach ~11.1V.
> 2.  **Update the code** thresholds to match a single-cell parallel voltage (e.g., Low = 3.5V, Critical = 3.2V).

*   **INA219 Placement:** The INA219 is now placed on the high-side between the battery bank and the Buck Converter to monitor the total power draw and battery discharge rate.
*   **Breadboard Setup:** The ESP32 should be centered on a standard breadboard, ensuring the I2C lines (SDA/SCL) and sensor inputs are short and well-grounded.
*   **Common Ground:** Ensure the battery ground, ESP32 ground, and LED ground are all tied together at a single point (star ground) to prevent flickering.

