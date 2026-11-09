# Limitations and future work

## Limitations
* Remote monitoring and push alerts need a working internet connection. Local alerts still work without it.
* Sensor placement and calibration change the result and must be set for each site.
* Results are from bench simulations, not a field trial.

* The rules use fixed thresholds, so noise can cause false alarms.
* The AES key is fixed and the IV is zero; this is a demonstration of message protection only.

## Future work
* AI-assisted anomaly detection to reduce false alarms (proposed in the report, not yet built).
* An offline alert path such as GSM SMS or a local siren with a long-range radio.
* A trained model on collected sensor data.
* More sensors: gas, water level, temperature.
* Several nodes reporting to one dashboard.
* Battery and solar power for remote sites.
