# Testing and results

Tests were done on the bench with simulated events. The results below are qualitative; no field deployment was done.

| Test | Method | Observation |
| --- | --- | --- |
| Normal conditions | all sensors idle | flame sensor inactive, vibration at baseline, distance stable, accelerometer steady at gravity; values shown in the app |
| Fire | controlled flame at different distances | the flame sensor responded quickly; alert reached the app; small fluctuations near strong ambient light, so alerts needed consistent detection |
| Shaking | tapping and shaking the platform | vibration sensor and accelerometer both changed; the sensors responded to sudden shaking |
| Tilt | gradually inclining the ADXL335 | X and Y voltage changed in proportion to the angle; warning raised beyond the safe limit |
| Distance change | moving an obstacle in front of the ultrasonic sensor | distance measured correctly; sudden changes were flagged as abnormal |
| Alert delivery | Wi-Fi to Blynk | live values and push notifications arrived without noticeable delay on a stable network |

## Summary

* Multi-hazard detection works on one low-cost platform.
* Fixed thresholds can alert on short noise; sensor calibration for each site matters.
* Hardware cost stayed low because the parts are common and cheap.
