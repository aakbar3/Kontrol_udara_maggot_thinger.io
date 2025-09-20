/*
 * Maggot Smart Controller
 * =======================
 * Proyek IoT untuk monitoring dan kontrol suhu serta kelembapan
 * pada kandang maggot menggunakan ESP8266, sensor AHT10, LCD I2C,
 * heater, dan mist maker.
 *
 * Fitur:
 * - Monitoring suhu & kelembapan via LCD
 * - Kontrol otomatis heater & mist maker
 * - Kirim data ke Thinger.io
 * - Konfigurasi WiFi via WiFiManager (AP Mode)
 *
 * Dibuat untuk: Skripsi
 * Penulis: [Nama Anda]
 * Tanggal: 20 September 2025
 */

#include <ThingerESP8266.h>
#include <Adafruit_AHTX0.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiManager.h>

// === Konfigurasi Thinger.io ===
ThingerESP8266 thing("Madar", "esp8266maggot", "maggot2121");

// === Inisialisasi Perangkat Keras ===
LiquidCrystal_I2C lcd(0x27, 16, 2);   // LCD 16x2 I2C
Adafruit_AHTX0 aht;                   // Sensor AHT10/AHT20
WiFiManager wifiManager;              // WiFiManager untuk setup WiFi

// === Pin Output ===
#define Heater_PIN 14  // D5
#define Mist_PIN   12  // D6

// === Variabel Global ===
int heat = 0, mist = 0;               // Status alat (0 = mati, 1 = nyala)
float suhu, kelembapan;               // Data sensor
String suhuStr, kelembapanStr;        // String untuk tampilan & kirim data

// === Interval Waktu ===
unsigned long lastDataUpdateTime = 0;
unsigned long lastControlUpdateTime = 0;
const unsigned long dataUpdateInterval = 1000;     // 1 detik
const unsigned long controlUpdateInterval = 60000; // 1 menit

// === Fungsi Callback WiFiManager ===
void configModeCallback(WiFiManager *myWiFiManager) {
  lcd.clear();
  lcd.print("AP Mode: ");
  lcd.setCursor(0, 1);
  lcd.print(myWiFiManager->getConfigPortalSSID());
}

// === Setup Awal ===
void setup() {
  lcd.init();
  lcd.backlight();
  aht.begin();

  pinMode(Heater_PIN, OUTPUT);
  pinMode(Mist_PIN, OUTPUT);
  digitalWrite(Heater_PIN, HIGH); // Mati
  digitalWrite(Mist_PIN, HIGH);   // Mati

  lcd.clear();
  lcd.print("Connecting...");

  wifiManager.setTimeout(180); // 3 menit timeout
  wifiManager.setAPCallback(configModeCallback);

  if (!wifiManager.autoConnect("Maggot_AP")) {
    delay(2000);
    ESP.restart();
  }

  lcd.clear();
  lcd.print("Connected to:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.SSID());
  delay(3000);
  lcd.clear();

  tampilkanData();

  // Definisi resource untuk Thinger.io
  thing["Kondisi kandang"] >> [](pson &out) {
    out["suhu"] = suhuStr;
    out["kelembapan"] = kelembapanStr;
    out["mist"] = mist;
    out["heater"] = heat;
  };
}

// === Loop Utama ===
void loop() {
  unsigned long currentTime = millis();

  // Update data sensor setiap 1 detik
  if (currentTime - lastDataUpdateTime >= dataUpdateInterval) {
    lastDataUpdateTime = currentTime;
    thing.handle();
  }

  // Kontrol alat & kirim data setiap 1 menit
  if (currentTime - lastControlUpdateTime >= controlUpdateInterval) {
    lastControlUpdateTime = currentTime;
    tampilkanData();
    kontrolAlat();
    kirimData();
  }
}

// === Fungsi Tampilkan Data ke LCD ===
void tampilkanData() {
  sensors_event_t humidity, temp;
  bool success = aht.getEvent(&humidity, &temp);

  suhu = success ? round((temp.temperature * 10) / 10.0) : 0;
  kelembapan = success ? round(humidity.relative_humidity) : 0;

  suhuStr = String(suhu, 1);
  kelembapanStr = String(kelembapan, 0);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.printf("Temp: %s C", suhuStr.c_str());
  lcd.setCursor(0, 1);
  lcd.printf("Humi: %s %%", kelembapanStr.c_str());
}

// === Fungsi Kontrol Alat ===
void kontrolAlat() {
  if (suhu < 30 || kelembapan > 80) {
    digitalWrite(Heater_PIN, LOW);  // Nyala
    digitalWrite(Mist_PIN, HIGH);   // Mati
    heat = 1;
    mist = 0;
  } else if (suhu >= 36 || kelembapan < 70) {
    digitalWrite(Mist_PIN, LOW);    // Nyala
    digitalWrite(Heater_PIN, HIGH); // Mati
    heat = 0;
    mist = 1;
  } else {
    digitalWrite(Heater_PIN, HIGH); // Mati
    digitalWrite(Mist_PIN, HIGH);   // Mati
    heat = 0;
    mist = 0;
  }
}

// === Fungsi Kirim Data ke Thinger.io ===
void kirimData() {
  pson data;
  data["suhu"] = suhuStr;
  data["kelembapan"] = kelembapanStr;
  data["mist"] = mist;
  data["heater"] = heat;

  thing.write_bucket("Skripsi", data);
}
