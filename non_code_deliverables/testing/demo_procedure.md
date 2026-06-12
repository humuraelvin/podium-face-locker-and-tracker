# Demo Procedure (Short Exam Run Script)

1. Power and wiring pre-check complete (12V path, buck outputs, common GND, capacitor placed).
2. Start MQTT broker on configured host (or verify remote broker is reachable).
3. Flash ESP8266 firmware and open serial monitor to confirm Wi-Fi + MQTT connection.
4. Run enrollment if profile not present.
5. Run tracker app.
6. Show face detection and confidence overlay.
7. Trigger lock; introduce second person to verify no switching.
8. Occlude speaker briefly; verify hold/scan/reacquire behavior.
9. Show runtime logs (`data/logs/runtime_log.csv`) with speaker ID, confidence, command, timestamp.
10. Safely stop software and power down hardware.
