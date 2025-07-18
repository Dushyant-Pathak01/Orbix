//Include required libraries  
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <time.h>
#include <ESPmDNS.h>

//Set the pin according to connections
#define LED_PIN D3
#define NUM_LEDS 12
#define BUTTON_PIN D8
#define BUTTON_PLUS_PIN D7
#define BUTTON_MINUS_PIN D9
#define BUZZER_PIN D4

// Replace the SSIDs with your Wi-fi Credetials
const char* ssids[] = {"SSID1", "SSID2"};
const char* passwords[] = {"PASS1", "PASS2"};
const int wifiCount = sizeof(ssids) / sizeof(ssids[0]);

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
WebServer server(80);

bool showClock = false;
bool pomodoroActive = false;
bool deviceOn = true;
int currentMode = 0;
int pomodoroState = 0;
unsigned long pomodoroStart = 0;
unsigned long pomodoroDurations[] = {1500000, 300000, 900000};
uint32_t candleColor = strip.Color(255, 147, 41);

unsigned long lastButtonDown = 0;
unsigned long lastButtonUp = 0;
unsigned long lastActionTime = 0;
int pressCount = 0;
bool buttonPressed = false;

unsigned long lastAnimUpdate = 0;

//Beep functions for Differnet beep behaviour on different condtions
void beep(int frequency, int duration) {
  int period = 1000000L / frequency;
  int pulse = period / 2;
  unsigned long endTime = millis() + duration;
  while (millis() < endTime) {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(pulse);
  }
}

void toneModeChange() {
  beep(3000, 150);
}

void tonePowerOn() {
  beep(1800, 150);
  delay(50);
  beep(2500, 180);
}

void tonePowerOff() {
  beep(2500, 180);
  delay(50);
  beep(1200, 150);
}

void tonePomodoro() {
  beep(1200, 100);
  delay(80);
  beep(1800, 100);
  delay(80);
  beep(2300, 100);
}

// Wifi Connection establishment
void connectToWiFi() {
  strip.fill(candleColor, 0, NUM_LEDS);
  strip.show();
  for (int i = 0; i < wifiCount; i++) {
    WiFi.begin(ssids[i], passwords[i]);
    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 8000) {
      delay(500);
    }
    if (WiFi.status() == WL_CONNECTED) {
      MDNS.begin("esp32");
      return;
    }
  }
}

//Current - Time fetching from NTP Server 
void initTime() {
  configTime(19800, 0, "pool.ntp.org", "time.nist.gov");
  while (time(nullptr) < 100000) {
    delay(100);
  }
}

// Different clock faces and animations
void showClockFace() {
  time_t now = time(nullptr);
  struct tm *t = localtime(&now);
  int h = t->tm_hour % 12;
  int m = t->tm_min / 5;
  int s = t->tm_sec / 5;
  strip.clear();
  strip.setPixelColor(h, strip.Color(255, 0, 0));
  strip.setPixelColor(m, strip.Color(0, 255, 0));
  strip.setPixelColor(s, strip.Color(0, 0, 255));
  strip.show();
}

void showPomodoro() {
  unsigned long now = millis();
  unsigned long elapsed = now - pomodoroStart;
  int progress = map(elapsed, 0, pomodoroDurations[pomodoroState], 0, NUM_LEDS);
  strip.clear();
  for (int i = 0; i < progress && i < NUM_LEDS; i++) {
    if (pomodoroState == 0) strip.setPixelColor(i, strip.Color(0, 255, 0));
    else if (pomodoroState == 1) strip.setPixelColor(i, strip.Color(0, 0, 255));
    else strip.setPixelColor(i, strip.Color(255, 0, 0));
  }
  strip.show();
  if (elapsed > pomodoroDurations[pomodoroState]) {
    pomodoroState = (pomodoroState + 1) % 3;
    pomodoroStart = now;
    tonePomodoro();
  }
}

void showStartupChase() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, candleColor);
    strip.show();
    delay(30);
    strip.setPixelColor(i, 0);
  }
  strip.clear();
  strip.show();
  tonePowerOn();
}

//Web page Elements -- which is hosted by ESP 32 C6 on local Ip and ESPmDNS 

void handleRoot() {
  String html = "<html><body><h2>ESP32 LED Control</h2>";
  html += "<form action='/mode'><button type='submit'>Next Mode</button></form>";
  html += "<form action='/clock'><button type='submit'>Show Clock</button></form>";
  html += "<form action='/off'><button type='submit'>Turn Off</button></form>";
  html += "<form action='/pomodoro'><button type='submit'>Start Pomodoro</button></form>";
  html += "<form action='/plus'><button type='submit'>+</button></form>";
  html += "<form action='/minus'><button type='submit'>-</button></form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleMode() {
  currentMode = (currentMode + 1) % 3;
  showClock = false;
  pomodoroActive = false;
  toneModeChange();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleClock() {
  showClock = true;
  pomodoroActive = false;
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleOff() {
  strip.clear();
  strip.show();
  showClock = false;
  pomodoroActive = false;
  server.sendHeader("Location", "/");
  server.send(303);
}

void handlePomodoro() {
  pomodoroActive = true;
  pomodoroStart = millis();
  pomodoroState = 0;
  showClock = false;
  tonePomodoro();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handlePlus() {
  if (pomodoroActive) pomodoroDurations[pomodoroState] += 60000;
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleMinus() {
  if (pomodoroActive && pomodoroDurations[pomodoroState] > 60000)
    pomodoroDurations[pomodoroState] -= 60000;
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PLUS_PIN, INPUT_PULLUP);
  pinMode(BUTTON_MINUS_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  strip.begin();
  strip.show();

  showStartupChase();
  connectToWiFi();
  initTime();

  server.on("/", handleRoot);
  server.on("/mode", handleMode);
  server.on("/clock", handleClock);
  server.on("/off", handleOff);
  server.on("/pomodoro", handlePomodoro);
  server.on("/plus", handlePlus);
  server.on("/minus", handleMinus);
  server.begin();
}

void loop() {
  server.handleClient();
  unsigned long now = millis();

  bool buttonRead = !digitalRead(BUTTON_PIN);

  if (buttonRead && !buttonPressed) {
    buttonPressed = true;
    lastButtonDown = now;
    pressCount++;
  }
  if (!buttonRead && buttonPressed) {
    buttonPressed = false;
    lastButtonUp = now;
  }

  if (!buttonRead && pressCount > 0 && (now - lastButtonUp > 400)) {
    if (pressCount == 1) {
      currentMode = (currentMode + 1) % 3;
      showClock = false;
      pomodoroActive = false;
      toneModeChange();
    } else if (pressCount == 2) {
      pomodoroActive = !pomodoroActive;
      showClock = !pomodoroActive;
      tonePomodoro();
    }
    pressCount = 0;
  }

  if (buttonPressed && (now - lastButtonDown > 2000)) {
    deviceOn = !deviceOn;
    if (deviceOn) {
      tonePowerOn();
    } else {
      strip.clear();
      strip.show();
      tonePowerOff();
    }
    pressCount = 0;
    buttonPressed = false;
    delay(1000);
  }

  if (!deviceOn) return;

  if (showClock && !pomodoroActive) showClockFace();
  else if (pomodoroActive) showPomodoro();
  else if (currentMode == 0) {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, candleColor);
    }
    strip.show();
  }
  else if (currentMode == 1 && now - lastAnimUpdate > 50) {
    strip.clear();
    int t = millis() / 100;
    for (int i = 0; i < NUM_LEDS; i++) {
      uint8_t b = (sin(t / 10.0 + i) + 1.0) * 127;
      strip.setPixelColor(i, strip.Color(b, b / 2, 0));
    }
    strip.show();
    lastAnimUpdate = now;
  }
  else if (currentMode == 2 && now - lastAnimUpdate > 100) {
    static int pos = 0;
    strip.clear();
    strip.setPixelColor(pos, candleColor);
    strip.show();
    pos = (pos + 1) % NUM_LEDS;
    lastAnimUpdate = now;
  }
}
