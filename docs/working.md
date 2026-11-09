# Working principle

## Loop (every 0.5 s)
1. Check Wi-Fi every 5 seconds; reconnect and reconnect Blynk if needed; show ONLINE or OFFLINE mode on a change.
2. Read the GPS serial stream and keep the last valid latitude and longitude.
3. Read the flame, vibration, ultrasonic and ADXL335 sensors.
4. Apply the rules and pick one alert.
5. Show NORMAL on the LCD when nothing has happened for 5 seconds.

## Rules and priority
Checked in this order, and the first match wins:

1. Fire: flame sensor LOW
2. Flood: ultrasonic distance under 7 cm (0 means no echo and is ignored)
3. Landslide: any ADXL335 axis above 2000
4. Earthquake: vibration sensor HIGH

## Alert
* LCD shows DISASTER ALERT and the hazard.
* Buzzer on for 2 seconds.
* DFPlayer plays track 1 (fire), 2 (flood), 3 (landslide) or 4 (earthquake).
* The alert text is encrypted with AES-128 (CBC) and shown on the serial monitor.
* If online: V0 encrypted text, V1 plain text, V2 latitude, V3 longitude, V4 map link are written, and a Blynk event (`info1` to `info4`) sends the push notification with the map link.
* The same alert is not repeated within 2 seconds.

## Offline behaviour
Without Wi-Fi the device keeps sensing and gives the LCD, buzzer and voice alerts. The cloud part resumes when Wi-Fi returns.

```
 sensors -> rule match? -> alert -> LCD + buzzer + voice
                              |
                              +-> online? -> encrypt, GPS, Blynk push
```
