# MQTT Servo Tracking Addon

> **Full start-to-finish test guide:** see the [main README](../../README.md) — stages 0–10 cover enrollment, lock, tracking, SEARCH, dashboard, and evidence logging.

This folder adds **MQTT motor control** and **structured evidence** on top of `src/recognize.py`.

## Quick commands

```powershell
# 1. Servo + MQTT only (no AI)
python addons/mqtt_servo_tracking/scripts/mqtt_servo_test.py

# 2. Full pipeline (after enrollment)
python addons/mqtt_servo_tracking/recognize_mqtt.py --target-name Elvin

# 3. Flash ESP32
powershell -ExecutionPolicy Bypass -File addons/mqtt_servo_tracking/esp32/upload.ps1 -Port COM6
```

## Files

| File | Role |
|------|------|
| `recognize_mqtt.py` | Recognition + lock + track + MQTT + JSONL evidence |
| `scripts/mqtt_servo_test.py` | Publish LEFT/RIGHT/CENTER/SCAN test sequence |
| `esp32/face_tracker.ino` | ESP32 subscriber (primary) |
| `esp32/upload.ps1` | arduino-cli compile + upload |
| `esp8266/face_tracker_servo/` | Legacy ESP8266 firmware |
| `../../dashboard/index.html` | BENAX live dashboard |

## MQTT defaults

| Item | Value |
|------|-------|
| Broker | `157.173.101.159:1883` |
| Movement | `vision/elvin/movement` |
| Status | `vision/elvin/status` |
| Dashboard WS | `ws://157.173.101.159:8083/mqtt` |

## Movement payloads

| Command | When |
|---------|------|
| `LEFT` | Locked speaker left of center |
| `RIGHT` | Locked speaker right of center |
| `CENTER` | Speaker in center band |
| `SCAN` | Locked speaker out of frame (after `--scan-delay-sec`) |
| `IDLE` | Not locked or waiting |

## Evidence logs

```text
logs/evidence/face_tracking_evidence_[timestamp].jsonl
```

Enabled by default. Use `--disable-evidence-log` only for informal tests.

## Recommended run (assessment)

```powershell
python addons/mqtt_servo_tracking/recognize_mqtt.py --target-name Elvin --mqtt-broker 157.173.101.159 --mqtt-port 1883 --mqtt-topic vision/elvin/movement --mqtt-status-topic vision/elvin/status --camera-width 960 --camera-height 540 --max-faces 5 --locked-max-faces 5 --detect-every 2 --recognize-every 3 --landmark-roi-width 224 --deadzone-px 70 --center-zone-ratio 0.36 --center-exit-hysteresis-px 45 --error-smooth-alpha 0.35 --command-hold-sec 0.25 --scan-delay-sec 0.8 --reacquire-hold-sec 0.30 --command-confirm-frames 2 --mqtt-min-interval 0.15 --mqtt-status-min-interval 0.25
```

Press **l** to lock Elvin. Leave the frame to trigger **SCAN**; return to re-acquire.

See [main README](../../README.md) for wiring, validation checklist, and troubleshooting.
