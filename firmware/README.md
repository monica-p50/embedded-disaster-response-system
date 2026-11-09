# Firmware

`disaster_response/disaster_response.ino` runs on an ESP32 (Arduino framework).

## Libraries
Install through the Arduino Library Manager:
* Blynk (BlynkSimpleEsp32)
* LiquidCrystal_I2C
* TinyGPSPlus
* DFRobotDFPlayerMini
* WiFi and mbedtls come with the ESP32 board package.

## Setup
1. Copy `secrets.h.example` to `secrets.h` in the same folder and fill in your Wi-Fi, Blynk template/token and a 16-character AES key. `secrets.h` is ignored by git.
2. In Blynk create a template with datastreams V0 to V4 (strings) and four events with codes `info1` to `info4` (fire, flood, landslide, earthquake).
3. Select an ESP32 board, upload, and open the serial monitor at 9600 baud.

## Pin map

| Function | ESP32 pin |
| --- | --- |
| Flame sensor (digital) | 34 |
| Vibration sensor (digital) | 25 |
| Ultrasonic TRIG / ECHO | 13 / 9 |
| ADXL335 X / Y / Z (analog) | 35 / 32 / 33 |
| Buzzer | 26 |
| GPS UART2 (RX / TX) | 16 / 17 |
| DFPlayer Mini UART1 (RX / TX) | 27 / 14 |
| LCD 16x2 I2C | address 0x27 (SDA/SCL default) |

## Blynk virtual pins

| Pin | Value |
| --- | --- |
| V0 | encrypted alert (hex) |
| V1 | decrypted alert text |
| V2 / V3 | latitude / longitude |
| V4 | Google Maps link |

## Detection rules in this firmware

| Alert | Rule | Voice file |
| --- | --- | --- |
| Fire | flame sensor output LOW | track 1 |
| Flood | ultrasonic distance under 7 cm (and not 0) | track 2 |
| Landslide | any ADXL335 axis reading above 2000 (12-bit ADC) | track 3 |
| Earthquake | vibration sensor HIGH | track 4 |

Alerts are checked in the order fire, flood, landslide, earthquake. The same alert is not repeated within 2 seconds. The buzzer sounds for 2 seconds.

## Notes
* The AES-128 (CBC) step encrypts the alert text and decrypts it again on the device to show the idea of protecting the message. The key is a fixed value and the IV is zero, so it is a demonstration, not production security.
* On many ESP32 DevKit boards GPIO 9 is used by the flash chip. If the ultrasonic ECHO pin does not work, move it to a free pin and update `#define echo`.
