# Assessment Checklist Mapping (A/B/C/D)

## A) Project Requirement Analysis
- Problem/objectives/scope/non-functional requirements: `non_code_deliverables/analysis/project_requirement_analysis.md`
- Architecture and data flow visual proof: `non_code_deliverables/architecture/system_block_diagram.svg`, `non_code_deliverables/architecture/recognize_track_command_flow.svg`
- Speaker lock conditions and safety logic: `src/vision/speaker_lock.py`, `non_code_deliverables/analysis/project_requirement_analysis.md`

## B) Preliminary Activities
- Environment setup + folder structure: `README.md`, repository tree.
- Camera readiness: `scripts/camera_test.py`
- Face library readiness: `requirements.txt`, enrollment/tracking modules.
- Motion setup and integration readiness: `firmware/esp8266_platformio/src/main.cpp`, `scripts/mqtt_pub_test.py`

## C) Product Implementation and Exhibition
- Core runtime and face tracking: `src/app/run_tracking.py`
- Enrollment and profile generation: `src/vision/enrollment.py`
- Lock feature and no-face/reacquisition behavior: `src/vision/speaker_lock.py`, `src/app/run_tracking.py`
- Motion quality (deadband/smoothing/proportional command): `src/control/motion_controller.py`
- MQTT command path and logging evidence: `src/mqtt_client/publisher.py`, `src/logging_utils/evidence_logger.py`
- Test scripts and demonstration procedure: `non_code_deliverables/testing/demo_procedure.md`, `non_code_deliverables/testing/test_case_matrix.md`

## D) Closing Activities
- Final run + handover actions: `non_code_deliverables/handover/closing_checklist.md`
- User operational guidance: `README.md`
