# 🐛 Maggot Smart Controller

Proyek IoT untuk **monitoring dan kontrol otomatis suhu serta kelembapan** pada kandang maggot menggunakan **ESP8266**, **sensor AHT10**, **LCD I2C**, **heater**, dan **mist maker**.

---

## 📸 Fitur Utama

- ✅ Monitoring suhu & kelembapan real-time via LCD 16x2
- ✅ Kontrol otomatis heater & mist maker berdasarkan nilai ambang
- ✅ Kirim data ke cloud (Thinger.io)
- ✅ Setup WiFi mudah via WiFiManager (AP Mode)
- ✅ Kode rapi dan berkomentar untuk pembelajaran

---

## 🧰 Komponen yang Digunakan

| Komponen         | Deskripsi                        |
|------------------|----------------------------------|
| ESP8266 (NodeMCU)| Mikrokontroler utama             |
| Sensor AHT10/20   | Sensor suhu & kelembapan         |
| LCD 16x2 I2C      | Tampilan data real-time          |
| Heater            | Untuk menaikkan suhu             |
| Mist Maker        | Untuk menaikkan kelembapan       |
| Relay Module      | Saklar elektronik (opsional)     |

---

## 🔌 Pin Wiring

| Perangkat   | Pin ESP8266 |
|-------------|-------------|
| Heater      | D5 (GPIO 14)|
| Mist Maker  | D6 (GPIO 12)|
| LCD I2C     | D2 (SDA), D1 (SCL) |
| AHT10       | D2 (SDA), D1 (SCL) |

&gt; Catatan: LCD dan AHT10 bisa dipakai bersama di I2C karena alamatnya berbeda.

---

## 📲 Setup WiFi

1. Nyalakan ESP8266
2. Jika WiFi belum tersimpan, akan muncul **AP Mode** dengan nama: `Maggot_AP`
3. Hubungkan HP/laptop ke `Maggot_AP`
4. Buka browser → otomatis muncul portal setup
5. Pilih jaringan WiFi dan masukkan password
6. Selesai! ESP akan restart dan otomatis tersambung

---

## ☁️ Thinger.io Setup

1. Buat akun di [https://thinger.io](https://thinger.io)
2. Buat device baru → salin **Username**, **Device Name**, dan **Credentials**
3. Update di baris ini:
   ```cpp
   ThingerESP8266 thing("USERNAME", "DEVICE_NAME", "CREDENTIALS");
