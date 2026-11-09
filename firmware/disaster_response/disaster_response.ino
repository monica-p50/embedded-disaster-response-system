// Private settings (Wi-Fi, Blynk token, AES key) live in secrets.h, which is not published.
// Copy secrets.h.example to secrets.h and fill in your own values.
#include "secrets.h"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "mbedtls/aes.h"
#include <TinyGPS++.h>
#include <DFRobotDFPlayerMini.h>

// WiFi
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Sensors
#define flame 34
#define trig 13
#define echo 9
#define vibration 25

#define adxlX 35
#define adxlY 32
#define adxlZ 33

#define buzzer 26
 
// GPS
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

// MP3
HardwareSerial mp3Serial(1);
DFRobotDFPlayerMini player;

// Variables
long duration;
int distance;

bool wifiConnected = false;
bool previousWifiStatus = false;

// AES Key
const char* key = AES_KEY;   // 16 characters

// Alert control
unsigned long lastAlertTime = 0;
String lastAlert = "";

// Buzzer control
unsigned long buzzerStartTime = 0;
bool buzzerActive = false;

// WiFi reconnect timer
unsigned long lastWiFiCheck = 0;

// GPS Data
String latitude = "0.000000";
String longitude = "0.000000";

// ---------------- AES ----------------
void encryptAES(unsigned char *input, unsigned char *output)
{
  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, (const unsigned char*)key, 128);
  unsigned char iv[16] = {0};
  mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, 16, iv, input, output);
  mbedtls_aes_free(&aes);
}

void decryptAES(unsigned char *input, unsigned char *output)
{
  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_dec(&aes, (const unsigned char*)key, 128);
  unsigned char iv[16] = {0};
  mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, 16, iv, input, output);
  mbedtls_aes_free(&aes);
}

String bytesToHex(unsigned char *data, int len)
{
  String hexStr = "";
  char buf[3];
  for (int i = 0; i < len; i++) {
    sprintf(buf, "%02X", data[i]);
    hexStr += buf;
  }
  return hexStr;
}

// ---------------- BUZZER ----------------
void startBuzzer()
{
  digitalWrite(buzzer, HIGH);
  Serial.println("🔊 Buzzer ON");
  buzzerStartTime = millis();
  buzzerActive = true;
}

void updateBuzzer()
{
  if (buzzerActive && millis() - buzzerStartTime >= 2000) {
    digitalWrite(buzzer, LOW);
    Serial.println("🔇 Buzzer OFF");
    buzzerActive = false;
  }
}

// ---------------- WIFI STATUS ----------------
void showWiFiStatus()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" NETWORK STATUS");

  lcd.setCursor(0, 1);
  if (wifiConnected)
    lcd.print("  ONLINE MODE    ");
  else
    lcd.print("  OFFLINE MODE   ");

  Serial.print("WiFi Status: ");
  Serial.println(wifiConnected ? "ONLINE" : "OFFLINE");
}

// ---------------- WIFI CHECK ----------------
void checkWiFiConnection()
{
  if (millis() - lastWiFiCheck < 5000) return;
  lastWiFiCheck = millis();

  previousWifiStatus = wifiConnected;

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;

    if (!Blynk.connected()) {
      Blynk.config(BLYNK_AUTH_TOKEN);
      Blynk.connect(2000);
    }
  } else {
    wifiConnected = false;
    WiFi.disconnect();
    WiFi.begin(ssid, pass);
  }

  if (wifiConnected != previousWifiStatus) {
    showWiFiStatus();
    delay(2000);
    lcd.clear();
  }
}

// ---------------- ALERT ----------------
void sendAlert(String plainText, String lcdMsg, String eventCode)
{
  if (lastAlert == plainText && millis() - lastAlertTime < 2000) return;

  lastAlert = plainText;
  lastAlertTime = millis();

  Serial.println("\n⚠️ ALERT TRIGGERED ⚠️");
  Serial.print("Type: ");
  Serial.println(plainText);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" DISASTER ALERT");
  lcd.setCursor(0, 1);
  lcd.print(lcdMsg);

  startBuzzer();

  // MP3 ALERT
  if (plainText == "FIRE ALERT") player.play(1);
  else if (plainText == "FLOOD ALERT") player.play(2);
  else if (plainText == "LANDSLIDE") player.play(3);
  else if (plainText == "EARTHQUAKE") player.play(4);

  unsigned char input[16] = {0};
  unsigned char encrypted[16] = {0};
  unsigned char decrypted[16] = {0};

  plainText.getBytes(input, 16);

  encryptAES(input, encrypted);
  decryptAES(encrypted, decrypted);

  String encryptedHex = bytesToHex(encrypted, 16);
  String decryptedText = String((char*)decrypted);

  Serial.print("Encrypted: ");
  Serial.println(encryptedHex);

  Serial.print("Decrypted: ");
  Serial.println(decryptedText);

  Serial.print("Latitude: ");
  Serial.println(latitude);

  Serial.print("Longitude: ");
  Serial.println(longitude);

  if (wifiConnected && Blynk.connected()) {

    String mapLink = "https://maps.google.com/?q=" + latitude + "," + longitude;

    Blynk.virtualWrite(V0, encryptedHex);
    Blynk.virtualWrite(V1, decryptedText);
    Blynk.virtualWrite(V2, latitude);
    Blynk.virtualWrite(V3, longitude);
    Blynk.virtualWrite(V4, mapLink);

    Blynk.logEvent(eventCode.c_str(), plainText + " Location: " + mapLink);
  }
}

// ---------------- NORMAL ----------------
void showNormalStatus()
{
  lcd.setCursor(0, 0);
  lcd.print(" SYSTEM NORMAL ");
  lcd.setCursor(0, 1);
  lcd.print(" ALL SAFE      ");

  Serial.println("✅ SYSTEM NORMAL");
}

// ---------------- SETUP ----------------
void setup()
{
  Serial.begin(9600);
  Serial.println("🚀 System Starting...");

  pinMode(flame, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(vibration, INPUT);
  pinMode(buzzer, OUTPUT);

  lcd.init();
  lcd.backlight();

  // GPS
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);

  // MP3
  mp3Serial.begin(9600, SERIAL_8N1, 27, 14);
  if (player.begin(mp3Serial)) {
    player.volume(25);
  }

  lcd.print("Connecting WiFi");
  WiFi.begin(ssid, pass);
  delay(3000);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Connected ✅");
    wifiConnected = true;
    Blynk.config(BLYNK_AUTH_TOKEN);
    Blynk.connect(2000);
  } else {
    Serial.println("WiFi Failed ❌");
  }

  showWiFiStatus();
  delay(2000);
  lcd.clear();
}

// ---------------- LOOP ----------------
void loop()
{
  checkWiFiConnection();

  if (wifiConnected && Blynk.connected())
    Blynk.run();

  updateBuzzer();

  // GPS UPDATE
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isValid()) {
    latitude = String(gps.location.lat(), 6);
    longitude = String(gps.location.lng(), 6);
  }

  // SENSOR READ
  int fire = digitalRead(flame);
  int vib = digitalRead(vibration);

  int x = analogRead(adxlX);
  int y = analogRead(adxlY);
  int z = analogRead(adxlZ);

  // ULTRASONIC
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH, 30000);
  distance = duration * 0.034 / 2;

  Serial.println("\n------ SENSOR DATA ------");
  Serial.print("Flame: "); Serial.println(fire);
  Serial.print("Vibration: "); Serial.println(vib);
  Serial.print("Distance: "); Serial.println(distance);
  Serial.print("X: "); Serial.println(x);
  Serial.print("Y: "); Serial.println(y);
  Serial.print("Z: "); Serial.println(z);

  bool fireDetected = (fire == LOW);
  bool floodDetected = (distance < 7 && distance != 0);
  bool landslideDetected = (x > 2000 || y > 2000 || z > 2000);
  bool earthquakeDetected = (vib == HIGH);

  if (fireDetected) {
    sendAlert("FIRE ALERT", "FOREST FIRE!", "info1");
  }
  else if (floodDetected) {
    sendAlert("FLOOD ALERT", "FLOOD ALERT!", "info2");
  }
  else if (landslideDetected) {
    sendAlert("LANDSLIDE", "LANDSLIDE!", "info3");
  }
  else if (earthquakeDetected) {
    sendAlert("EARTHQUAKE", "EARTHQUAKE!", "info4");
  }
  else {
    if (millis() - lastAlertTime > 5000) {
      showNormalStatus();
      lastAlert = "";
    }
  }

  delay(500);
}