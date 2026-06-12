# Project Requirement Analysis (Assessment Section A)

## 1) Problem Definition and Objectives
- **Problem statement:** Conventional trackers follow any visible face; this project requires speaker-specific tracking in live podium scenarios.
- **Main objective:** Build an AI-based single-speaker lock camera tracking system with real-time motorized pan control.
- **Real-world relevance:** Supports lectures/presentations where audience/co-presenters must be ignored while tracking the designated speaker.
- **Expected outcome:** Stable autonomous camera pan that maintains enrolled speaker near frame center and provides evidence logs.

## 2) System Scope and Requirements
- **Scope in this implementation:** one-axis horizontal tracking only (pan), single enrolled identity, MQTT-based motor control, evidence logging.
- **Functional requirements:**
  - enrollment (10 to 30 images),
  - face detection in each frame,
  - speaker recognition against stored embedding,
  - lock/unlock behavior and reacquisition,
  - control command publishing to ESP8266,
  - motor actuation via A4988.
- **Non-functional requirements:**
  - low command latency,
  - smooth movement with deadband/smoothing,
  - stable behavior under moderate lighting variation,
  - continuous runtime without crashes.
- **Assumptions/limitations:**
  - one-axis movement,
  - recognition quality depends on enrollment quality and lighting,
  - no physical end-stop sensors (software limits + careful mechanical setup).

## 3) Architecture and Components
- **Hardware components:** USB camera, PC, Wemos D1 Mini ESP8266, A4988, NEMA17, 12V PSU, buck converter, common ground, 100 uF capacitor near VMOT.
- **Software components:** enrollment module, recognition/tracking module, lock state machine, motion controller, MQTT publisher, firmware subscriber, logger.
- **System diagram reference:** `non_code_deliverables/architecture/system_block_diagram.svg`.

## 4) Data Flow and Control Logic
- Camera frames -> face detection -> embedding comparison -> lock state update.
- Lock-confirmed target -> horizontal error -> smoothing/deadband -> command generation.
- Command payload -> MQTT broker -> ESP8266 -> DIR/STEP pulses -> motor rotation.
- Multi-face behavior: only enrolled identity accepted; others ignored.
- No-face behavior: safe stop or controlled scan for reacquisition based on lock state.

## 5) Speaker Lock Logic and Safety
- **Lock activation:** automatic after configurable consecutive accepted frames or manual key trigger.
- **Lock hold:** when additional faces appear, continue tracking locked identity.
- **Unlock conditions:** manual unlock or target lost longer than timeout.
- **Safety measures:** common GND, regulated rails, VMOT capacitor, speed limits, stop-on-command-timeout in firmware.
