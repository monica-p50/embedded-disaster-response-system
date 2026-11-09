# Hardware

| Part | Job |
| --- | --- |
| ESP32 | reads sensors, runs the logic, Wi-Fi to the cloud |
| Flame sensor | infrared from a flame, digital output |
| Vibration sensor | ground or structural vibration, digital output |
| ADXL335 | 3-axis acceleration and tilt, analog outputs read by the ADC |
| Ultrasonic sensor | distance by echo time: D = (T x C) / 2, C about 343 m/s |
| GPS module (NMEA over UART) | latitude and longitude for the alert |
| DFPlayer Mini + speaker | plays a stored voice message per hazard |
| 16x2 I2C LCD | shows status and alerts locally |
| Buzzer | audible alarm |
| Power supply | 12 V source regulated to 5 V by a 7805 |

See `firmware/README.md` for the pin map.

## Power supply
The mains version uses a step-down transformer (about 12 V), a bridge rectifier of four 1N4001 diodes and a filter capacitor, then a 7805 regulator for 5 V. A 12 V battery can replace the transformer, and a switch controls the power.

```
 230 V AC -> transformer -> bridge rectifier -> filter -> 7805 -> 5 V
 12 V battery ---------------------------------------> 7805 -> 5 V
```

## Sensor notes
* **Flame sensor:** an infrared receiver with a comparator. Strong ambient light can cause small fluctuations.
* **ADXL335:** at rest it reads gravity; tilting changes the X and Y voltages in proportion to the angle.
* **Ultrasonic:** sends a pulse and times the echo. A short distance suggests rising water.
* **Vibration sensor:** gives a signal when vibration passes its sensitivity setting.
