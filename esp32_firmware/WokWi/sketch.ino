#include "SmartServo.h"
#include "SmartServoController.h"
#include "MqttSmartServoBridge.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

static const int SERVO_PIN = 18;
static const char *SERVO_ID = "servo1";
static const char *WIFI_SSID = "Wokwi-GUEST";
static const char *WIFI_PASS = "";
static const char *MQTT_BROKER = "test.mosquitto.org";
static const char *MQTT_CMD_TOPIC = "ROS_To_ESP/cmd";
static const char *MQTT_STATE_TOPIC = "ESP_To_ROS/state";
static const char *MQTT_CLIENT_ID = "esp32_client_servo1";

// MQTT command format:
//   <speed>,<direction>
//   speed: float, positive magnitude
//   direction: 0 = counter-clockwise, 1 = clockwise
// Example: "0.5,1" means move at 0.5 speed clockwise.
//
// Note: Wokwi and the physical servo are limited to 0..180 degrees.
// The internal `motorAngle` model can run continuously, but the actual
// servo output is mapped into the 0..180 range for hardware display.

SmartServo servo(SERVO_PIN);
SmartServoController servoController(&servo);
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//SmartServo servo_2(4);
//SmartServoController SmartServoController_2(&servo_2);
MqttSmartServoBridge mqtt;
unsigned long lastTime = 0;
char stateMessage[128];

bool endpointDetectionEnabled = true;
bool torqueSimulationEnabled = true;

bool endpointEnforcementEnabled = false; // False = Task1 , True = Task 2 


void applyTestModes()
{
    if (!endpointEnforcementEnabled) {
        endpointDetectionEnabled = false;
    }

    servoController.setEndpointDetectionEnabled(endpointDetectionEnabled);
    servo.setEndpointEnforcement(endpointEnforcementEnabled);
    servoController.setTorqueSimulationEnabled(torqueSimulationEnabled);
}

void setup()
{
    setupSerial();
    setupServo();
    setupMqtt();
    setupDisplay();

    lastTime = millis();
    Serial.println("SYSTEM STARTED");
}

void loop()
{
    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0;
    lastTime = now;

    mqtt.loop();
    processMqttCommands();
    servoController.run(dt);
    publishServoState();
    renderDisplay();
    delay(100);
}

void setupSerial()
{
    Serial.begin(115200);
}

void setupServo()
{
    servo.begin();
    applyTestModes();
    if (endpointEnforcementEnabled) {
        servo.generateRandomEndpoints();
    }
    //applyTestModes();
}

void setupMqtt()
{
    mqtt.begin(WIFI_SSID, WIFI_PASS, MQTT_BROKER, MQTT_CLIENT_ID);
    mqtt.subscribe(MQTT_CMD_TOPIC);
    Serial.print("Subscribed to MQTT topic: ");
    Serial.println(MQTT_CMD_TOPIC);
}

void setupDisplay()
{
    Wire.begin();
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED init failed");
        while (1);
    }
    display.clearDisplay();
    display.display();
}

void processMqttCommands()
{
    if (!mqtt.hasNewMessage()) {
        return;
    }

    String incoming = mqtt.getMessage();

    // Parse MQTT command payload as CSV: <speed>,<direction>
    // This code does not support any additional mode or config messages.
    int commaIndex = incoming.indexOf(',');
    if (commaIndex <= 0) {
        return;
    }

    float cmd = incoming.substring(0, commaIndex).toFloat();
    int dir = incoming.substring(commaIndex + 1).toInt();

    servoController.setCommand(cmd);
    servoController.setCommandDirection(dir == 1);
}

void publishServoState()
{
    snprintf(stateMessage, sizeof(stateMessage), "%.3f,%.3f,%.3f,%d",
             servoController.getMotorAngle(),
             servoController.getDoorAngle(),
             servoController.getSpeed(),
             servoController.getCommandedDirection());
    mqtt.publish(MQTT_STATE_TOPIC, stateMessage);
}

void renderDisplay()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);

    display.print("Servo: ");
    display.println(SERVO_ID);
    display.print("Motor Ang: ");
    display.println(servoController.getMotorAngle());

    display.print("Door Ang: ");
    display.println(servoController.getDoorAngle());

    display.print("Speed: ");
    display.println(servo.getSpeed());

    display.print("Direction: ");
    display.println(servo.getCommandedDirection() ? "CW" : "Counter-CW");

    if (servoController.isDetectionDone()) {
        display.print("Det Open: ");
        display.println(servoController.getDetectedOpenAngle());
        display.print("Det Closed: ");
        display.println(servoController.getDetectedClosedAngle());
        display.print("Mode: ");
        display.println("Normal");
    } else {
        display.print("OpenAng Lim: ");
        display.println(servoController.getDoorAngleOpenLimit());
        display.print("ClosedAng Lim: ");
        display.println(servoController.getDoorAngleClosedLimit());
        display.print("Torque Detecting: ");
        display.println(servo.getSimulatedTorque());
        // display.print("Mode: ");
        // display.println("Detecting...");
    }

    display.display();
}

