# Test Case Matrix (Assessment Sections B and C)

| ID | Test | Procedure | Expected Result | Evidence |
|---|---|---|---|---|
| TC-01 | Camera feed | Run camera test script | Live feed appears without crash | Screenshot/video |
| TC-02 | Enrollment | Run enrollment and capture >=10 samples | Profile saved to `data/enrollment` | File timestamps + console |
| TC-03 | Recognition | Start tracker with enrolled speaker only | Bounding box and confidence shown | Screen capture |
| TC-04 | Multi-face lock | Add second person after lock | System keeps tracking enrolled speaker | Video + logs |
| TC-05 | No-face safety | Hide face from camera | STOP or scan behavior shown | Logs + serial output |
| TC-06 | Reacquisition | Return enrolled face after occlusion | Same speaker reacquired | Video + logs |
| TC-07 | Motor direction | Move speaker left/right in frame | Motor rotates correct direction | Video + observer notes |
| TC-08 | Smoothness | Perform slow movement across podium | Motion is smooth with low jitter | Video |
| TC-09 | MQTT link | Disconnect/reconnect broker/Wi-Fi | System reconnects and resumes control | Serial monitor |
| TC-10 | Reliability run | 10-minute continuous demo | No crash, consistent response | Runtime logs |
