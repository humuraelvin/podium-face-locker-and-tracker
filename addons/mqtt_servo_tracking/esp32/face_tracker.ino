#define USE_US_TIMER

#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

const char* WIFI_SSID = "Peace And Love Ploclaimers";
const char* WIFI_PASSWORD = "loveisthekey";
// =========================
// MQTT Settings
// =========================
const char* MQTT_SERVER = "157.173.101.159";
const uint16_t MQTT_PORT = 1883;

const char* MQTT_TOPIC = "vision/elvin/movement";
// Unique per board; chip ID appended in setup().
String mqttClientId = "elvin-face-servo";

// =========================
// Servo Configuration
// =========================
const uint8_t SERVO_PIN = 18; // Recommended ESP32 pin

const int SERVO_MIN_ANGLE = 0;
const int SERVO_MAX_ANGLE = 180;
const int SERVO_CENTER_ANGLE = 90;

const int TRACK_STEP = 5;
const int SEARCH_STEP = 5;

const unsigned long TRACK_INTERVAL_MS = 80;
const unsigned long SEARCH_INTERVAL_MS = 180;
const unsigned long COMMAND_TIMEOUT_MS = 8000;

const bool REVERSE_SERVO = true;

// =========================
// Command Types
// =========================
enum MovementCommand {
  CMD_IDLE,
  CMD_LEFT,
  CMD_RIGHT,
  CMD_CENTER,
  CMD_SEARCH
};

// =========================
// Global Objects
// =========================
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
Servo panServo;

// =========================
// State Variables
// =========================
MovementCommand currentCommand = CMD_IDLE;

int servoAngle = SERVO_CENTER_ANGLE;
int sweepDirection = 1;

unsigned long lastMoveAt = 0;
unsigned long lastReconnectAttempt = 0;
unsigned long lastCommandAt = 0;

// ======================================================
// Servo Functions
// ======================================================

void setServoAngle(int angle) {
  angle = constrain(angle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

  servoAngle = angle;
  panServo.write(servoAngle);
}

void applyTrackingStep(int logicalDirection) {

  int direction =
      REVERSE_SERVO ? -logicalDirection : logicalDirection;

  setServoAngle(
      servoAngle + (direction * TRACK_STEP)
  );
}

// ======================================================
// Command Parsing
// ======================================================

MovementCommand parseCommand(String message) {

  message.trim();
  message.toUpperCase();

  // Allow CMD_LEFT style
  if (message.startsWith("CMD_")) {
    message = message.substring(4);
  }

  if (message == "LEFT") {
    return CMD_LEFT;
  }

  if (message == "RIGHT") {
    return CMD_RIGHT;
  }

  if (message == "CENTER") {
    return CMD_CENTER;
  }

  // Python tracker publishes SCAN; accept both names.
  if (message == "SEARCH" || message == "SCAN") {
    return CMD_SEARCH;
  }

  if (message == "IDLE") {
    return CMD_IDLE;
  }

  return CMD_IDLE;
}

// ======================================================
// MQTT Callback
// ======================================================

void mqttCallback(
    char* topic,
    byte* payload,
    unsigned int length
) {

  String message = "";

  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  currentCommand = parseCommand(message);

  lastCommandAt = millis();

  Serial.print("[MQTT] Received: ");
  Serial.println(message);
}

// ======================================================
// Serial Input
// ======================================================

void handleSerial() {

  if (Serial.available() > 0) {

    String input =
        Serial.readStringUntil('\n');

    input.trim();

    MovementCommand newCmd =
        parseCommand(input);

    currentCommand = newCmd;

    lastCommandAt = millis();

    Serial.print("[SERIAL] Executing: ");
    Serial.println(input);
  }
}

// ======================================================
// Wi-Fi Connection
// ======================================================

void connectWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.println("[WiFi] Connecting...");

  WiFi.mode(WIFI_STA);

  // Disconnect old attempts first
  WiFi.disconnect(true);

  delay(1000);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();

  while (
      WiFi.status() != WL_CONNECTED &&
      millis() - startAttemptTime < 15000
  ) {

    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println();
    Serial.println("[WiFi] Connected!");

    Serial.print("[WiFi] IP Address: ");
    Serial.println(WiFi.localIP());

  } else {

    Serial.println();
    Serial.println("[WiFi] Connection Failed");
  }
}
// ======================================================
// MQTT Connection
// ======================================================

bool connectMqtt() {

  if (mqttClient.connected()) {
    return true;
  }

  if (millis() - lastReconnectAttempt < 5000) {
    return false;
  }

  lastReconnectAttempt = millis();

  Serial.print("[MQTT] Connecting...");

  bool connected =
      mqttClient.connect(mqttClientId.c_str());

  if (!connected) {

    Serial.print("Failed, rc=");
    Serial.println(mqttClient.state());

    return false;
  }

  Serial.println("Connected");

  mqttClient.subscribe(MQTT_TOPIC);

  Serial.print("[MQTT] Subscribed to: ");
  Serial.println(MQTT_TOPIC);

  return true;
}

// ======================================================
// Servo Logic
// ======================================================

void handleServo() {

  unsigned long now = millis();

  // Auto idle timeout
  if ((now - lastCommandAt) >
      COMMAND_TIMEOUT_MS) {

    currentCommand = CMD_IDLE;
  }

  // ----------------------
  // CENTER
  // ----------------------
  if (currentCommand == CMD_CENTER) {

    setServoAngle(SERVO_CENTER_ANGLE);

    currentCommand = CMD_IDLE;

    return;
  }

  // ----------------------
  // SEARCH MODE
  // ----------------------
  if (currentCommand == CMD_SEARCH) {

    if (
        now - lastMoveAt <
        SEARCH_INTERVAL_MS
    ) {
      return;
    }

    lastMoveAt = now;

    setServoAngle(
        servoAngle +
        (sweepDirection * SEARCH_STEP)
    );

    if (servoAngle >= SERVO_MAX_ANGLE) {
      sweepDirection = -1;
    }

    if (servoAngle <= SERVO_MIN_ANGLE) {
      sweepDirection = 1;
    }

    return;
  }

  // ----------------------
  // LEFT / RIGHT TRACKING
  // ----------------------
  if (
      now - lastMoveAt <
      TRACK_INTERVAL_MS
  ) {
    return;
  }

  lastMoveAt = now;

  if (currentCommand == CMD_LEFT) {

    applyTrackingStep(-1);

  } else if (
      currentCommand == CMD_RIGHT
  ) {

    applyTrackingStep(1);
  }
}

// ======================================================
// Setup
// ======================================================

void runBootServoTest() {
  Serial.println("[TEST] Boot servo sweep (45 -> 135 -> 90)...");
  for (int angle = 45; angle <= 135; angle += 4) {
    setServoAngle(angle);
    delay(35);
  }
  for (int angle = 135; angle >= 45; angle -= 4) {
    setServoAngle(angle);
    delay(40);
  }
  setServoAngle(SERVO_CENTER_ANGLE);
  Serial.println("[TEST] Boot servo sweep done.");
}

void setup() {

  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println(
      "[SYS] ESP32 Face Servo Initializing..."
  );
  mqttClientId = String("elvin-face-servo-") + String((uint32_t)ESP.getEfuseMac(), HEX);
  Serial.print("[MQTT] Client ID: ");
  Serial.println(mqttClientId);

  // ----------------------
  // ESP32 Servo Setup
  // ----------------------
  ESP32PWM::allocateTimer(0);

  panServo.setPeriodHertz(50);

  panServo.attach(
      SERVO_PIN,
      500,
      2400
  );

  setServoAngle(SERVO_CENTER_ANGLE);
  runBootServoTest();

  // ----------------------
  // MQTT Setup
  // ----------------------
  mqttClient.setServer(
      MQTT_SERVER,
      MQTT_PORT
  );

  mqttClient.setCallback(mqttCallback);

  // ----------------------
  // WiFi
  // ----------------------
  connectWiFi();

  lastCommandAt = millis();
}

// ======================================================
// Main Loop
// ======================================================

void loop() {

  // Wi-Fi reconnect
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // MQTT reconnect
  if (!mqttClient.connected()) {
    connectMqtt();
  }

  mqttClient.loop();

  // Serial commands
  handleSerial();

  // Servo movement
  handleServo();

  delay(1);
}
