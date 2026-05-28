/********************************************************
 * =====================================================
 *            SMART SECURITY MONITORING SYSTEM
 * =====================================================
 *
 * BOARD       : ESP32
 * SENSOR      : DHT11, LDR, HC-SR04
 * OUTPUT      : LED, BUZZER
 * PLATFORM    : BLYNK IoT
 *
 * FUNGSI:
 * -----------------------------------------------------
 * 1. Membaca suhu ruangan menggunakan DHT11
 * 2. Membaca cahaya menggunakan LDR
 * 3. Mengukur jarak/pintu menggunakan HC-SR04
 * 4. Mengirim data ke aplikasi Blynk
 * 5. Menyalakan alarm jika kondisi berbahaya
 *
 ********************************************************/




/* =====================================================
 *                  KONFIGURASI BLYNK
 * =====================================================
 */


#define BLYNK_TEMPLATE_ID "TMPL6YGlgG6BU"
#define BLYNK_TEMPLATE_NAME "Tugas Case 1"
#define BLYNK_AUTH_TOKEN "1dp2XzlWT6d7guvUbIv7jVlrq3hQ93--"


// Menampilkan proses Blynk ke Serial Monitor
#define BLYNK_PRINT Serial




/* =====================================================
 *                      LIBRARY
 * =====================================================
 */


#include <WiFi.h>               // Library WiFi ESP32
#include <BlynkSimpleEsp32.h>  // Library Blynk ESP32
#include <Adafruit_Sensor.h>   // Library sensor umum
#include <DHT.h>               // Library sensor DHT
#include <DHT_U.h>             // Unified DHT
#include "Arduino.h"




/* =====================================================
 *                   WIFI SETTINGS
 * =====================================================
 */


char ssid[] = "Xiaomi 12 Lite"; // Nama WiFi
char pass[] = "123456789a";     // Password WiFi




/********************************************************
 * DHTTYPE DHT11
 *
 * DHT11 = tipe sensor suhu & kelembaban
 ********************************************************/


#define DHTTYPE DHT11




/* =====================================================
 *                    PIN CONFIG
 * =====================================================
 *
 * Angka dibawah adalah nomor GPIO ESP32
 */


#define DHTPIN      26  // GPIO 26 → Data DHT11
#define LDR_PIN     34  // GPIO 34 → Analog LDR
#define TRIG_PIN    4   // GPIO 4  → Trigger HC-SR04
#define ECHO_PIN    5   // GPIO 5  → Echo HC-SR04
#define LED_PIN     32  // GPIO 32 → LED indikator
#define BUZZER_PIN  23  // GPIO 23 → Buzzer




/* =====================================================
 *                MEMBUAT OBJECT DHT
 * =====================================================
 */


DHT_Unified dht(DHTPIN, DHTTYPE);




/* =====================================================
 *               VARIABEL GLOBAL
 * =====================================================
 *
 * Variabel global = bisa digunakan semua fungsi
 */


uint32_t delayMS;




/* =====================================================
 *                      SETUP
 * =====================================================
 *
 * setup() hanya berjalan 1 kali saat ESP32 hidup
 */


void setup() {


  /******************************************************
   * Serial.begin(115200)
   *
   * 115200 = baud rate
   * Satuan : bps (bit per second)
   *
   * Artinya:
   * ESP32 mengirim data serial
   * sebanyak 115200 bit per detik
   ******************************************************/


  Serial.begin(115200);




  // Memulai sensor DHT11
  dht.begin();




  /* ===================================================
   *                MODE PIN
   * ===================================================
   */


  pinMode(LDR_PIN, INPUT);      // LDR sebagai INPUT
  pinMode(LED_PIN, OUTPUT);     // LED sebagai OUTPUT
  pinMode(BUZZER_PIN, OUTPUT);  // Buzzer sebagai OUTPUT


  pinMode(TRIG_PIN, OUTPUT);    // Trigger ultrasonic OUTPUT
  pinMode(ECHO_PIN, INPUT);     // Echo ultrasonic INPUT




  /******************************************************
   * ledcSetup(channel, frekuensi, resolusi)
   *
   * ledcSetup(0, 2000, 8)
   *
   * 0     = channel PWM ESP32
   *
   * 2000  = frekuensi PWM
   *         satuan : Hertz (Hz)
   *
   *         Artinya:
   *         sinyal bergetar 2000 kali/detik
   *
   * 8     = resolusi PWM
   *         satuan : bit
   *
   *         8 bit = 256 level PWM
   *         karena:
   *
   *         2^8 = 256
   ******************************************************/


  ledcSetup(0, 2000, 8);




  // Menghubungkan buzzer ke channel PWM 0
  ledcAttachPin(BUZZER_PIN, 0);




  /* ===================================================
   *                  KONEKSI WIFI
   * ===================================================
   */


  WiFi.begin(ssid, pass);




  /* ===================================================
   *               KONEKSI BLYNK
   * ===================================================
   */


  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
}




/* =====================================================
 *                      LOOP
 * =====================================================
 *
 * loop() berjalan terus menerus
 */


void loop() {


  // Menjalankan sistem Blynk
  Blynk.run();




  /* ===================================================
   *                 SENSOR DHT11
   * ===================================================
   */


  sensors_event_t event;


  // Mengambil data suhu
  dht.temperature().getEvent(&event);


  // Menyimpan suhu ke variabel
  float dhtValue = event.temperature;


  /******************************************************
   * dhtValue
   *
   * Satuan:
   * derajat Celsius (°C)
   ******************************************************/




  /* ===================================================
   *                     SENSOR LDR
   * ===================================================
   */


  int ldrValue = analogRead(LDR_PIN);


  /******************************************************
   * analogRead()
   *
   * Membaca ADC ESP32
   *
   * Range ADC:
   * 0 - 4095
   *
   * Semakin kecil:
   * semakin terang
   *
   * Semakin besar:
   * semakin gelap
   ******************************************************/




  /* ===================================================
   *               SENSOR ULTRASONIC
   * ===================================================
   */


  // Memastikan trigger LOW
  digitalWrite(TRIG_PIN, LOW);


  /******************************************************
   * delayMicroseconds(2)
   *
   * Satuan:
   * mikrodetik (µs)
   *
   * 2 µs = 0.000002 detik
   ******************************************************/


  delayMicroseconds(2);




  // Mengirim trigger ultrasonic
  digitalWrite(TRIG_PIN, HIGH);


  /******************************************************
   * delayMicroseconds(10)
   *
   * Trigger aktif selama 10 mikrodetik
   *
   * HC-SR04 membutuhkan trigger minimal:
   * 10 µs
   ******************************************************/


  delayMicroseconds(10);




  // Mematikan trigger
  digitalWrite(TRIG_PIN, LOW);




  /******************************************************
   * pulseIn(ECHO_PIN, HIGH, 30000)
   *
   * 30000 = timeout
   * satuan : mikrodetik (µs)
   *
   * 30000 µs = 30 ms
   *
   * Jika tidak ada pantulan dalam 30 ms
   * maka pembacaan dihentikan
   ******************************************************/


  long duration = pulseIn(ECHO_PIN, HIGH, 30000);




  /******************************************************
   * Rumus Jarak:
   *
   * distance = duration × 0.034 / 2
   *
   * 0.034 = kecepatan suara
   * satuan : cm/µs
   *
   * Artinya:
   * suara bergerak 0.034 cm
   * setiap 1 mikrodetik
   *
   * Dibagi 2 karena:
   * gelombang pergi dan kembali
   ******************************************************/


  float distance = duration * 0.034 / 2;


  /******************************************************
   * distance
   *
   * Satuan:
   * centimeter (cm)
   ******************************************************/




  /* ===================================================
   *               MENGIRIM KE BLYNK
   * ===================================================
   */


  Blynk.virtualWrite(V0, dhtValue); // V0 = suhu
  Blynk.virtualWrite(V1, ldrValue); // V1 = cahaya
  Blynk.virtualWrite(V2, distance); // V2 = jarak




  /* ===================================================
   *               SERIAL MONITOR
   * ===================================================
   */


  Serial.println("==============================");


  Serial.println("Cahaya         : " + String(ldrValue));


  Serial.println("Suhu           : " + String(dhtValue) + " °C");


  Serial.println("Pintu Terbuka  : " + String(distance) + " cm");




  /* ===================================================
   *                SISTEM PERINGATAN
   * ===================================================
   */


  bool alert = false;




  /******************************************************
   * if (ldrValue < 2000)
   *
   * 2000 = batas cahaya
   * Range ADC = 0 - 4095
   *
   * Jika kurang dari 2000:
   * dianggap ada cahaya masuk
   ******************************************************/


  if (ldrValue < 2000) {


    Serial.println("[PERINGATAN] Cahaya terdeteksi!");


    alert = true;
  }




  /******************************************************
   * if (dhtValue > 24)
   *
   * 24 = batas suhu maksimum
   * satuan : °C
   ******************************************************/


  if (dhtValue > 24) {


    Serial.println("[PERINGATAN] Suhu terlalu panas!");


    alert = true;
  }




  /******************************************************
   * if (distance > 5)
   *
   * 5 = batas pintu terbuka
   * satuan : cm
   ******************************************************/


  if (distance > 5) {


    Serial.println("[PERINGATAN] Pintu terbuka!");


    alert = true;
  }




  /* ===================================================
   *            AKSI SAAT ADA PERINGATAN
   * ===================================================
   */


  if (alert) {


    // Menyalakan LED
    digitalWrite(LED_PIN, HIGH);




    /****************************************************
     * ledcWriteTone(0, 1000)
     *
     * 0    = channel PWM
     * 1000 = frekuensi suara buzzer
     *        satuan : Hertz (Hz)
     *
     * Artinya:
     * buzzer bergetar 1000 kali/detik
     ****************************************************/


    ledcWriteTone(0, 1000);




    /****************************************************
     * delay(1000)
     *
     * satuan : millisecond (ms)
     *
     * 1000 ms = 1 detik
     ****************************************************/


    delay(1000);




    // Mematikan LED
    digitalWrite(LED_PIN, LOW);




    /****************************************************
     * ledcWriteTone(0, 500)
     *
     * 500 Hz = nada lebih rendah
     ****************************************************/


    ledcWriteTone(0, 500);


  }


  else {


    // LED mati
    digitalWrite(LED_PIN, LOW);


    // Buzzer mati
    ledcWriteTone(0, 0);
  }




  /******************************************************
   * delay(1000)
   *
   * Delay pembacaan sensor
   *
   * 1000 ms = 1 detik
   ******************************************************/


  delay(1000);
}

