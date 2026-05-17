/**
 * @file        Smart_Street_Lighting_System.ino
 * @author      Mohamed Tarek
 * @date        03/05/2026
 * @version     3.1
 * @brief       Advanced IoT Street Lighting System with Telemetry and Web Control.
 * 
 * @description
 *              This firmware manages smart street lighting using an ESP32. It 
 *              features adaptive brightness based on LDR sensors, IR motion 
 *              detection, and battery health monitoring via INA219.
 *              
 *              Features: 
 *              - Adaptive brightness (LDR + Motion hold)
 *              - Dual-zone LED control (FastLED)
 *              - Web Dashboard for real-time telemetry
 *              - Battery & Power monitoring (INA219)
 *              - Environmental sensing (DHT22 + Rain Sensor)
 * 
 * @usage
 *              1. Ensure the ESP32 is connected to the 'Smart Street Lighting' network.
 *              2. Access the web dashboard via the local IP address.
 *              3. Monitor sensors or use the Override modes for manual control.
 * 
 * @note
 *              Required Libraries: FastLED, Adafruit_INA219, DHT, WebServer.
 * ----------------------------------------------------------------------------
 */

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <time.h>
#include <FastLED.h>
#include <DHT.h>
#include "dashboard.h"

// -----------------------------------------------------------------------------
//  Hardware
// -----------------------------------------------------------------------------
Adafruit_INA219 ina219;
bool            ina219Found = false;
WebServer       server(80);

// -----------------------------------------------------------------------------
//  WiFi & Static IP Configuration
// -----------------------------------------------------------------------------
const char* ssid     = "[NAME OF YOUR NETWORK]";
const char* password = "[PASSWORD]";

// Toggle between Router Mode and Mobile Hotspot Mode
#define USE_ROUTER_MODE true 

#if USE_ROUTER_MODE
  // --- Home/Office Router Config ---
  IPAddress local_IP(192, 168, 1, 12); 
  IPAddress gateway(192, 168, 1, 1);   
#else
  // --- Mobile Hotspot Config ---
  IPAddress local_IP(172, 16, 147, 12); 
  IPAddress gateway(172, 16, 147, 1);  
#endif

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   
IPAddress secondaryDNS(8, 8, 4, 4); 

// -----------------------------------------------------------------------------
//  AP Mode (Direct Connection)
// -----------------------------------------------------------------------------
const char* ssidAP     = "Smart Street Lighting";
const char* passwordAP = "12345678";

// -----------------------------------------------------------------------------
//  GPIO Pin Assignments
// -----------------------------------------------------------------------------
#define LDR_PIN         34  // Analog input
#define IR_PIN          26  // Digital input (IR Obstacle Sensor)
#define RAIN_PIN        25  // Digital input (supports internal pull-up)
#define DHT_PIN         4   // Digital pin for DHT22
#define LED_PIN_P1      5   // Pillar 1 Data
#define LED_PIN_P2      19  // Pillar 2 Data
#define LED_PIN_P3      13  // Pillar 3 Data
#define LED_PIN_P4      14  // Pillar 4 Data

#define LIGHT_BOX_PIN   33  // Light box LED 1
#define LIGHT_BOX_PIN2  32  // Light box LED Ground

#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);

// -----------------------------------------------------------------------------
//  FastLED Setup
// -----------------------------------------------------------------------------
#define NUM_LEDS 8
CRGB leds[NUM_LEDS];

// -----------------------------------------------------------------------------
//  Configurable Settings
// -----------------------------------------------------------------------------
int   LDR_THRESHOLD    = 2000;    // Below this = dark = lights on
float BATTERY_LOW      = 10.5f;   // Below this → reduce to half brightness
float BATTERY_CRITICAL = 9.0f;    // Below this → lock to minimum brightness
int   motionHoldTime   = 5000;   // ms to hold full brightness after motion

// Pre-computed rain & fog override colors (CRGB values, already scaled)
//   Rain: warm white ~3000K at 80% brightness
//   Fog : warm amber ~2700K at 55% brightness
#define RAIN_COLOR_R 204
#define RAIN_COLOR_G 157
#define RAIN_COLOR_B 114
#define FOG_COLOR_R  140
#define FOG_COLOR_G   88
#define FOG_COLOR_B   44

// -----------------------------------------------------------------------------
//  Runtime State (volatile for cross-core thread safety)
// -----------------------------------------------------------------------------
volatile bool systemPower       = true;
volatile bool overrideMode      = false;
volatile int  targetBrightness1 = 0;
volatile int  targetBrightness2 = 0;
volatile bool lightBoxState     = false;

//  Soft glow / breathing parameters
volatile bool softGlowEnabled = true;
volatile int  glowMin   = 50;
volatile int  glowMax   = 170;
volatile int  glowSpeed = 1;
volatile int  flashCounter = 0; // Thread-safe animation flag
volatile int  glowR     = 0;
volatile int  glowG     = 255;
volatile int  glowB     = 255;
volatile bool partyModeEnabled  = false;
unsigned long lastPartyMs       = 0;
const int     PARTY_INTERVAL    = 600;
int           currentR  = 0;
int           currentG  = 255;
int           currentB  = 255;

//  Environmental Sensing
float currentHumidity  = 0;
float currentTemp      = 0;
unsigned long lastDHTMs = 0;
const unsigned long DHT_INTERVAL = 2000;

//  Sensor cache — populated every loop(), consumed by /data endpoint
int   cachedLDR       = 0;
bool  cachedMotion    = false;
float cachedVoltage   = 0.0f;
float cachedCurrentMa = 0.0f;
bool  cachedRaining   = false;

//  WiFi state tracking for Light Box indicator
bool wasConnected = false;  // set by connectWiFi(), updated in loop()

//  Smooth fade
int           current1      = 0;
int           current2      = 0;
unsigned long lastFadeMs    = 0;
const int     FADE_INTERVAL = 10;   // ms between fade steps
const int     FADE_SPEED    = 3;    // brightness units per step

//  Breathing / soft glow
int           breathValue   = glowMin;
int           breathDir     = 1;
unsigned long lastBreathMs  = 0;
const int     BREATH_INTERVAL = 50; // ms between breathing steps

//  Motion timing (hold)
unsigned long lastMotionMs  = 0;

//  History sampling
#define HISTORY_SIZE 50
float voltageHistory[HISTORY_SIZE];
int   motionHistory[HISTORY_SIZE];
int   historyIndex  = 0;
unsigned long lastHistoryMs = 0;
const unsigned long HISTORY_INTERVAL = 5000; // sample every 5 s

//  WiFi reconnection
unsigned long lastWifiCheckMs = 0;
const unsigned long WIFI_CHECK_INTERVAL = 15000; // check every 15 s

//  Performance throttling
unsigned long lastSensorMs = 0;
const unsigned long SENSOR_INTERVAL = 100; // read sensors at 10Hz
unsigned long lastShowMs   = 0;
const unsigned long SHOW_INTERVAL   = 30;  // update LEDs at ~33 FPS

// =============================================================================
//  SETUP
// =============================================================================
void setup() {
  Serial.begin(115200);

  // GPIO
  pinMode(IR_PIN, INPUT_PULLUP); // IR module usually pulls LOW on detect
  pinMode(RAIN_PIN, INPUT_PULLUP); // Use pull-up to prevent floating when disconnected
  pinMode(LIGHT_BOX_PIN, OUTPUT);
  pinMode(LIGHT_BOX_PIN2, OUTPUT);
  digitalWrite(LIGHT_BOX_PIN, LOW);
  digitalWrite(LIGHT_BOX_PIN2, LOW);

  // FastLED
  // FastLED: 4 Independent Data Pins (one per pillar)
  // All controllers point to the same 'leds' array but with different offsets.
  // Pillar 1: LEDs 0-1, Pillar 2: LEDs 2-3 (Zone 1)
  // Pillar 3: LEDs 4-5, Pillar 4: LEDs 6-7 (Zone 2)
  FastLED.addLeds<WS2812B, LED_PIN_P1, RGB>(leds, 0, 2).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, LED_PIN_P2, RGB>(leds, 2, 2).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, LED_PIN_P3, RGB>(leds, 4, 2).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, LED_PIN_P4, RGB>(leds, 6, 2).setCorrection(TypicalLEDStrip);
  
  FastLED.setBrightness(255); // Explicitly set brightness to ensure visibility

  // Startup Flash: Prove LEDs work immediately
  fill_solid(leds, NUM_LEDS, CRGB::Cyan);
  FastLED.show();
  delay(1000); // Increased to 1.0s for clear visibility during startup
  FastLED.clear();
  FastLED.show();

  // DHT Sensor
  dht.begin();

  // INA219 current/voltage sensor
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C
  #if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 2
    Wire.setTimeOut(50);   // 50ms timeout to prevent hanging on flaky sensors
  #endif

  if (ina219.begin()) {
    ina219Found = true;
    Serial.println(F("[SUCCESS] INA219 initialized."));
  } else {
    Serial.println(F("WARNING: INA219 not found. Voltage reads will return 0."));
  }

  // WiFi
  connectWiFi();

  // Zero-out history buffers
  memset(voltageHistory, 0, sizeof(voltageHistory));
  memset(motionHistory,  0, sizeof(motionHistory));

  setupServer();
  server.begin();
  Serial.println(F("HTTP server started."));

  // ── Multi-threading ────────────────────────────────────────────────────────
  // Run the Web Server handle on Core 0 (background core)
  // Stack: 4096, Priority: 1, Core: 0
  xTaskCreatePinnedToCore(
    [](void* pvParameters) {
      for (;;) {
        server.handleClient();
        vTaskDelay(2 / portTICK_PERIOD_MS); // Yield to other system tasks
      }
    },
    "ServerTask", 8192, NULL, 1, NULL, 0
  );
  Serial.println(F("[CORE] Web Server pinned to Core 0."));
}

// =============================================================================
//  LOOP
// =============================================================================
void loop() {
  // server.handleClient(); // Now handled by ServerTask on Core 0

  // --- WiFi Connection Handshake (NTP, MDNS, etc.) ---
  bool isConnected = (WiFi.status() == WL_CONNECTED);
  if (isConnected && !wasConnected) {
    // One-time initialization after connection
    Serial.println(F("\n[SUCCESS] WiFi Connected!"));
    WiFi.setSleep(false); // Max responsiveness
    configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    if (MDNS.begin("Smart-Street-Lighting")) {
       MDNS.addService("http", "tcp", 80);
    }
    
    lightBoxState = false;
    digitalWrite(LIGHT_BOX_PIN,  LOW);
    digitalWrite(LIGHT_BOX_PIN2, LOW);
    wasConnected = true;
  } else if (!isConnected && wasConnected) {
    lightBoxState = true;
    digitalWrite(LIGHT_BOX_PIN,  HIGH);
    digitalWrite(LIGHT_BOX_PIN2, LOW);
    wasConnected = false;
  }

  // --- Periodic Reconnection Check ---
  if (millis() - lastWifiCheckMs >= WIFI_CHECK_INTERVAL) {
    lastWifiCheckMs = millis();
    if (!isConnected && WiFi.getMode() != WIFI_MODE_NULL) {
      WiFi.reconnect(); 
    }
  }

  // --- Read sensors periodically (at SENSOR_INTERVAL) ---
  if (millis() - lastSensorMs >= SENSOR_INTERVAL) {
    lastSensorMs = millis();
    cachedLDR       = analogRead(LDR_PIN);
    bool irMotion   = (digitalRead(IR_PIN) == LOW); // IR is LOW when detected
    cachedMotion    = irMotion;
    if (ina219Found) {
      cachedVoltage   = ina219.getBusVoltage_V();
      cachedCurrentMa = ina219.getCurrent_mA();
    }
    cachedRaining   = (digitalRead(RAIN_PIN) == LOW);
  }

  // Read DHT periodically (slow sensor, 2 s minimum interval)
  if (millis() - lastDHTMs >= DHT_INTERVAL) {
    lastDHTMs = millis();
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (!isnan(h)) currentHumidity = h;
    if (!isnan(t)) currentTemp = t;
  }

  // Motion hold: track last time motion actually fired
  if (cachedMotion) lastMotionMs = millis();
  bool motionActive = (millis() - lastMotionMs < (unsigned long)motionHoldTime);

  // --- Sample history at a fixed rate ---
  if (millis() - lastHistoryMs >= HISTORY_INTERVAL) {
    lastHistoryMs = millis();
    voltageHistory[historyIndex] = cachedVoltage;
    motionHistory[historyIndex]  = cachedMotion ? 1 : 0;
    historyIndex++;
    if (historyIndex >= HISTORY_SIZE) historyIndex = 0;
  }

  // --- Decide target brightness (autonomous mode) ---
  if (!systemPower) {
    targetBrightness1 = 0;
    targetBrightness2 = 0;
  } else if (!overrideMode) {
    if (cachedLDR < LDR_THRESHOLD) {
      targetBrightness1 = adaptiveGlow(cachedVoltage, motionActive);
      targetBrightness2 = targetBrightness1;
    } else {
      targetBrightness1 = 0;
      targetBrightness2 = 0;
    }
  }

  // --- Party Mode Backend ---
  if (partyModeEnabled) {
    if (millis() - lastPartyMs >= PARTY_INTERVAL) {
      lastPartyMs = millis();
      currentR = random(256);
      currentG = random(256);
      currentB = random(256);
      glowR = currentR; glowG = currentG; glowB = currentB;
      targetBrightness1 = 255;
      targetBrightness2 = 255;
      overrideMode = true;
      systemPower = true;
    }
  }

  if (flashCounter <= 0) {
    handleFade();
    handleBreathing(motionActive);
  }
  handlePhysicalFlash(); // Safely check for flash requests from server

  // --- Push LED updates at a controlled frame rate ---
  if (millis() - lastShowMs >= SHOW_INTERVAL) {
    lastShowMs = millis();
    FastLED.show();
  }
}

// =============================================================================
//  FIRMWARE FUNCTIONS
// =============================================================================

// WiFi connect / reconnect helper
void connectWiFi() {
  Serial.println(F("\n--- WiFi Connection Start ---"));
  
  // Turn ON Light Box while connecting/disconnected
  lightBoxState = true;
  digitalWrite(LIGHT_BOX_PIN, HIGH);
  digitalWrite(LIGHT_BOX_PIN2, LOW);

  // Clean start: clear previous connection state
  WiFi.disconnect(true); 
  delay(1000);
  
  // Set Dual Mode: Connect to Hotspot AND create own WiFi
  WiFi.mode(WIFI_AP_STA);
  
  // Create Access Point (for easy direct access)
  WiFi.softAP(ssidAP, passwordAP); 
  Serial.print(F("Access Point Started. IP: "));
  Serial.println(WiFi.softAPIP());
  
  Serial.print(F("Connecting to SSID: "));
  Serial.println(ssid);
  
  // Set hostname before begin
  WiFi.setHostname("Smart-Street-Lighting");
  WiFi.hostname("Smart-Street-Lighting"); 
  
  // Configures static IP (Call BEFORE begin for faster/more stable connection)
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println(F("[ERROR] Static IP configuration failed!"));
  }

  WiFi.begin(ssid, password);
  Serial.println(F("WiFi connection initiated (Non-blocking)."));
}

// ---------------------------------------------------------------------------
// Adaptive glow logic — priority (lowest → highest):
//   1. Time-of-day sets the baseline
//   2. Motion boost overrides time dimming (safety)
//   3. Battery critically caps the output regardless of motion
// ---------------------------------------------------------------------------
int adaptiveGlow(float voltage, bool motionActive) {
  // 1. Time-of-day baseline
  int base = glowMax; // default: full brightness
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    int hour = timeinfo.tm_hour;
    if (hour >= 0  && hour < 6)  base = (glowMin + glowMax) / 2; // midnight dim
    if (hour >= 6  && hour < 18) base = 0;                         // daytime
    if (hour >= 18)              base = glowMax;                   // evening: full
  }

  // 2. Motion boost (overrides time dimming for safety)
  if (motionActive) base = glowMax;

  // 3. Battery protection cap (highest priority — always respected)
  if (voltage <= BATTERY_CRITICAL) {
    base = min(base, (int)glowMin);                  // critical: lock to minimum
  } else if (voltage <= BATTERY_LOW) {
    base = min(base, (int)(glowMin + glowMax) / 2);  // low: cap at half
  }

  return base;
}

// ---------------------------------------------------------------------------
// Smooth fade handler — drives current1/2 toward target each FADE_INTERVAL ms
// ---------------------------------------------------------------------------
void handleFade() {
  if (millis() - lastFadeMs >= FADE_INTERVAL) {
    lastFadeMs = millis();

    // 1. Determine target colors (weather overrides user color)
    int targetR = glowR, targetG = glowG, targetB = glowB;
    bool foggy = (currentHumidity > 90.0f);
    if (cachedRaining) {
      targetR = RAIN_COLOR_R; targetG = RAIN_COLOR_G; targetB = RAIN_COLOR_B;
    } else if (foggy) {
      targetR = FOG_COLOR_R;  targetG = FOG_COLOR_G;  targetB = FOG_COLOR_B;
    }

    // 2. Fade colors smoothly
    if (currentR < targetR) currentR++;
    else if (currentR > targetR) currentR--;
    if (currentG < targetG) currentG++;
    else if (currentG > targetG) currentG--;
    if (currentB < targetB) currentB++;
    else if (currentB > targetB) currentB--;

    // 3. Fade Brightness
    // Zone 1
    if      (current1 < targetBrightness1) current1 = min(current1 + FADE_SPEED, (int)targetBrightness1);
    else if (current1 > targetBrightness1) current1 = max(current1 - FADE_SPEED, (int)targetBrightness1);

    // Zone 2
    if      (current2 < targetBrightness2) current2 = min(current2 + FADE_SPEED, (int)targetBrightness2);
    else if (current2 > targetBrightness2) current2 = max(current2 - FADE_SPEED, (int)targetBrightness2);

    // 4. Render LEDs
    int out1 = current1;
    int out2 = current2;
    // Breathing takes over when the system has settled to the idle low-light state
    if (softGlowEnabled && !overrideMode && !motionActive && current1 == targetBrightness1 && targetBrightness1 > 0) out1 = breathValue;
    if (softGlowEnabled && !overrideMode && !motionActive && current2 == targetBrightness2 && targetBrightness2 > 0) out2 = breathValue;

    applyZonePWM(1, out1);
    applyZonePWM(2, out2);
  }
}

// ---------------------------------------------------------------------------
// Soft-glow breathing  — BUG FIX: now has its own interval so it does not
// fire every loop and overwrite the fade output.
// Only active when soft glow is enabled, not in override mode, and
// both zones are already at their target (idle glow).
// ---------------------------------------------------------------------------
void handleBreathing(bool motionActive) {
  if (!softGlowEnabled || overrideMode || motionActive) return;
  if (targetBrightness1 == 0 && targetBrightness2 == 0) return;

  if (millis() - lastBreathMs >= BREATH_INTERVAL) {
    lastBreathMs = millis();
    breathValue += breathDir * glowSpeed;
    if (breathValue >= glowMax) { breathValue = glowMax; breathDir = -1; }
    if (breathValue <= glowMin) { breathValue = glowMin; breathDir =  1; }
  }
}

// ---------------------------------------------------------------------------
// Thread-safe Physical Flash Animation
// ---------------------------------------------------------------------------
void handlePhysicalFlash() {
  if (flashCounter <= 0) return;
  static unsigned long lastFlashStepMs = 0;
  if (millis() - lastFlashStepMs >= 200) {
    lastFlashStepMs = millis();
    if (flashCounter % 2 == 0) {
      fill_solid(leds, NUM_LEDS, CRGB::White);
    } else {
      FastLED.clear();
    }
    // Note: FastLED.show() is called by the main loop throttling logic below
    flashCounter--;
  }
}

// ---------------------------------------------------------------------------
// Apply RGB PWM to a zone, scaled by brightness (0–255).
// Weather overrides use pre-computed color constants (see top of file).
// Rain/fog state is read from the sensor cache, not the pin, to avoid
// redundant digitalRead() calls inside this hot-path function.
// ---------------------------------------------------------------------------
void applyZonePWM(int zone, int brightness) {
  brightness = constrain(brightness, 0, 255);
  
  // Scale the smoothly faded color by the current brightness level
  int finalR = map(currentR, 0, 255, 0, brightness);
  int finalG = map(currentG, 0, 255, 0, brightness);
  int finalB = map(currentB, 0, 255, 0, brightness);

  if (zone == 1) {
    for (int i = 0; i < 4; i++) {
      if (i < NUM_LEDS) leds[i] = CRGB(finalR, finalG, finalB);
    }
  } else if (zone == 2) {
    for (int i = 4; i < 8; i++) {
      if (i < NUM_LEDS) leds[i] = CRGB(finalR, finalG, finalB);
    }
  }
}

// =============================================================================
//  WEB SERVER
// =============================================================================

void setupServer() {

  // ── /status — Consolidated telemetry (1s - 2s) ────────────────────────────
  server.on("/status", []() {
    String json;
    json.reserve(400); // Pre-allocate to avoid fragmentation
    json = "{";
    // Power
    json += "\"voltage\":"     + String(cachedVoltage, 2)    + ",";
    json += "\"current\":"     + String(cachedCurrentMa, 1)  + ",";
    // Sensors
    json += "\"ldr\":"         + String(cachedLDR)           + ",";
    json += "\"motion\":"      + String(cachedMotion ? 1 : 0) + ",";
    json += "\"temp\":"        + String(currentTemp, 1)      + ",";
    json += "\"humid\":"       + String(currentHumidity, 1)  + ",";
    json += "\"raining\":"     + String(cachedRaining ? 1 : 0) + ",";
    // System
    json += "\"power\":"       + String(systemPower ? 1 : 0)     + ",";
    json += "\"z1\":"          + String(current1)                + ",";
    json += "\"z2\":"          + String(current2)                + ",";
    json += "\"mode\":\""      + String(overrideMode ? "override" : "auto") + "\",";
    json += "\"glow\":"        + String(softGlowEnabled ? 1 : 0) + ",";
    json += "\"box\":"         + String(lightBoxState ? 1 : 0)   + ",";
    json += "\"r\":"           + String(glowR)               + ",";
    json += "\"g\":"           + String(glowG)               + ",";
    json += "\"b\":"           + String(glowB)               + ",";
    json += "\"party\":"       + String(partyModeEnabled ? 1 : 0);
    json += "}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
  });

  // ── /history — Large history arrays (fetch every 10s) ──────────────────────
  server.on("/history", []() {
    String json = "{";
    json += "\"voltageHistory\":[";
    for (int i = 0; i < HISTORY_SIZE; i++) {
      json += String(voltageHistory[i], 2);
      if (i < HISTORY_SIZE - 1) json += ",";
    }
    json += "],\"motionHistory\":[";
    for (int i = 0; i < HISTORY_SIZE; i++) {
      json += String(motionHistory[i]);
      if (i < HISTORY_SIZE - 1) json += ",";
    }
    json += "]}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
  });

  // ── /on — Override: both zones full brightness ────────────────────────────
  server.on("/on", []() {
    partyModeEnabled  = false;
    systemPower       = true;
    overrideMode      = true;
    targetBrightness1 = 255;
    targetBrightness2 = 255;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "ON");
  });

  // ── /off — Override: both zones off ──────────────────────────────────────
  server.on("/off", []() {
    partyModeEnabled  = false;
    systemPower       = false;
    overrideMode      = true;
    targetBrightness1 = 0;
    targetBrightness2 = 0;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "OFF");
  });

  // ── /auto — Return to autonomous control ─────────────────────────────────
  server.on("/auto", []() {
    partyModeEnabled = false;
    systemPower  = true;
    overrideMode = false;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "AUTO");
  });

  // ── /setZone?z=1&b=128 — Override individual zone brightness ─────────────
  server.on("/setZone", []() {
    if (!systemPower) {
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(400, "text/plain", "System OFF");
      return;
    }
    partyModeEnabled = false;
    int zone = server.arg("z").toInt();
    if (zone != 1 && zone != 2) {
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(400, "text/plain", "Bad zone");
      return;
    }
    int brightness = constrain(server.arg("b").toInt(), 0, 255);
    if (zone == 1) targetBrightness1 = brightness;
    else           targetBrightness2 = brightness;
    overrideMode = true;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "OK");
  });

  // ── /setColor?r=0&g=255&b=255 — Set RGB glow color ───────────────────────
  server.on("/setColor", []() {
    if (!systemPower) {
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(400, "text/plain", "System OFF");
      return;
    }
    partyModeEnabled = false;
    glowR = constrain(server.arg("r").toInt(), 0, 255);
    glowG = constrain(server.arg("g").toInt(), 0, 255);
    glowB = constrain(server.arg("b").toInt(), 0, 255);
    
    if (server.hasArg("snap") && server.arg("snap") == "1") {
      currentR = glowR;
      currentG = glowG;
      currentB = glowB;
    }

    // Force override so user can see the color change immediately
    overrideMode = true; 
    // If they were off, turn them on to at least 25% brightness
    if (targetBrightness1 < 64) targetBrightness1 = 64;
    if (targetBrightness2 < 64) targetBrightness2 = 64;

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "OK");
  });

  // ── /glowToggle — Toggle soft breathing glow ──────────────────────────────
  server.on("/glowToggle", []() {
    softGlowEnabled = !softGlowEnabled;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", softGlowEnabled ? "1" : "0");
  });

  // ── /partyToggle — Toggle backend Party Mode ──────────────────────────────
  server.on("/partyToggle", []() {
    partyModeEnabled = !partyModeEnabled;
    if (partyModeEnabled) {
      systemPower = true;
      overrideMode = true;
      targetBrightness1 = 255;
      targetBrightness2 = 255;
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", partyModeEnabled ? "1" : "0");
  });

  // ── /toggleLightBox — Toggle Light Box ───────────────────────────────────
  server.on("/toggleLightBox", []() {
    lightBoxState = !lightBoxState;
    digitalWrite(LIGHT_BOX_PIN,  lightBoxState ? HIGH : LOW);
    digitalWrite(LIGHT_BOX_PIN2, LOW);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", lightBoxState ? "1" : "0");
  });

  // ── /flash — Flash physical LEDs for identification ──────────────────────
  server.on("/flash", []() {
    flashCounter = 10; // Trigger 5 blinks (on/off states)
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "FLASHING");
  });

  // ── / — Dashboard HTML ────────────────────────────────────────────────────
  server.on("/", []() {
    server.send_P(200, "text/html", DASHBOARD_HTML);
  });
}

