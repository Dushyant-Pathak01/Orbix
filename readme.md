# Orbix

**Orbix** is a smart ESP32-C6-powered LED ring project that blends ambient lighting, Pomodoro productivity, real-time clock visuals, and Wi-Fi-based web control into a compact, interactive device.

---

## 🌟 Features

- **Candle-style ambient lighting**
- **Pomodoro Timer** with visual LED progress and buzzer alerts
- **Analog Clock Mode** using 12 NeoPixels
- **Interactive Web Interface** for:
  - Brightness control
  - Mode switching
  - Power toggle
- **Hardware Button Controls** with:
  - Brightness +/−
  - Power on/off via long press
  - Mode switching via short press
  - Pomodoro toggle via double press
- **Custom Buzzer Sounds** for feedback
- **Multi-WiFi SSID Auto-Connect**
---

## 🧠 Modes

| Mode         | Description                                      |
|--------------|--------------------------------------------------|
| Candle Glow  | Flickering candle-like ambient light             |
| Animation    | Color flow and motion lighting pattern           |
| Dot Chase    | Single LED chasing animation                     |
| Clock Mode   | Live time visualization (hour, min, sec hands)   |
| Pomodoro     | Focus timer with LED progress + buzzer alerts    |

---

## 🔧 Hardware Requirements

- **ESP32-C6 Dev Board**
- **12x WS2812 (NeoPixel) LEDs** in a ring
- **Momentary Push Buttons** ×3 (Mode/Power, +, −)
- **Passive Buzzer**
- **500mAh LiPo battery or 5V power supply**

---

## 📲 Web Control UI

Accessible over Wi-Fi. Features:

- Mode selection
- Power On/Off
- Pomodoro start
- Clock display

Just connect to the same network and visit:  
``http://esp32.local`` (if mDNS is supported)  
Or check your Serial Monitor or router for the assigned IP.

---

## 🔌 Wiring -- According to SeedStudio Xiao ESP 32-C6

| Component        | Pin           |
|------------------|---------------|
| LED Ring         | GPIO 21 (D3)  |
| Button (Mode)    | GPIO 19 (D8)  |
| Button (+)       | GPIO 17 (D7)  |
| Button (−)       | GPIO 20 (D9)  |
| Buzzer           | GPIO 22 (D4)  |

---

## 🚀 Getting Started

1. Install the following Arduino libraries:
   - Adafruit NeoPixel
   - ESPAsyncWebServer
   - ArduinoOTA
2. Set board to **ESP32-C6 Dev Module**
3. Replace Wi-Fi SSIDs and passwords:

```cpp
const char* ssids[] = {"SSID1", "SSID2"};
const char* passwords[] = {"PASS1", "PASS2"};
```

4. Upload and open the Serial Monitor to get IP or use `esp32.local`.

---

## 🔔 Sounds

- **Power On/Off** – 2 short tones
- **Mode Change** – Dual high-pitch chirp
- **Pomodoro Switch** – 3-tone chime

---

## 📁 Files Included

- `Orbix.ino` — Main source code (optimized & structured) -- in /Firmware
- `README.md` — This file
- diagram files -- in /images

---

## 🧠 Name Inspiration

**Orbix** = *Orb* (circle, light, clock) + *ix* (suffix)  
Represents a glowing orb that’s interactive, ambient, and smart.

---

## 📜 License

MIT License – Use freely, modify, and share with credits 🙌

---

## ✨ Author

Made with ❤️ by Dushyant Pathak  
Inspired by simplicity, productivity, and aesthetic ambient tech.
