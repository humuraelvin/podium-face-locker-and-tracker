#!/usr/bin/env python3
"""Publish test MQTT movement commands to verify ESP32 servo wiring."""

from __future__ import annotations

import json
import sys
import time
from pathlib import Path

try:
    import paho.mqtt.client as mqtt
except ImportError:
    print("Install dependencies: pip install paho-mqtt")
    raise SystemExit(1)

ADDON_ROOT = Path(__file__).resolve().parents[1]
CONFIG_PATH = ADDON_ROOT / "esp32" / "device_config.json"


def load_config() -> dict:
    if CONFIG_PATH.exists():
        return json.loads(CONFIG_PATH.read_text(encoding="utf-8"))
    return {
        "mqtt": {
            "broker": "157.173.101.159",
            "port": 1883,
            "movement_topic": "vision/elvin/movement",
        }
    }


def publish_sequence(client: mqtt.Client, topic: str) -> None:
    commands = [
        ("CENTER", 1.5),
        ("RIGHT", 4.0),
        ("LEFT", 4.0),
        ("RIGHT", 4.0),
        ("CENTER", 1.5),
        ("SCAN", 5.0),
        ("IDLE", 1.0),
    ]
    for command, hold_sec in commands:
        print(f"Publishing {command} -> {topic}")
        client.publish(topic, command, qos=0, retain=False)
        time.sleep(hold_sec)


def main() -> int:
    cfg = load_config()
    mqtt_cfg = cfg.get("mqtt", {})
    broker = mqtt_cfg.get("broker", "157.173.101.159")
    port = int(mqtt_cfg.get("port", 1883))
    topic = mqtt_cfg.get("movement_topic", "vision/elvin/movement")

    client = mqtt.Client(client_id="facelock-pc-test", clean_session=True)
    print(f"Connecting to {broker}:{port} ...")
    client.connect(broker, port, keepalive=30)
    client.loop_start()
    time.sleep(1.0)

    print("Starting servo test sequence (watch the motor)...")
    publish_sequence(client, topic)

    client.loop_stop()
    client.disconnect()
    print("Done. If the servo moved, MQTT + ESP32 setup is working.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
