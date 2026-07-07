#pragma once
#include "ISmartServo.h"

class SmartServoController {
public:
    enum class DetectionState { SEEK_OPEN, SEEK_CLOSE, DONE };

private:
    ISmartServo* smartServo;

    DetectionState detectionState = DetectionState::SEEK_OPEN;
    float detectedOpenAngle   = 0.0f;
    float detectedClosedAngle = 0.0f;
    bool endpointDetectionEnabled = true;

    static constexpr float DETECTION_SPEED     = 0.5f;
    static constexpr float TORQUE_THRESHOLD    = 8.0f;

    void runEndpointDetection(float dt);

public:
    explicit SmartServoController(ISmartServo* servo);

    // Lifecycle and Run Loop
    void run(float dt);

    // Commands and Directions
    void setCommand(float cmd);
    void setCommandDirection(bool clockwise);
    bool getCommandedDirection() const;

    // Motion and Feedback State
    float getMotorAngle() const;
    float getDoorAngle() const;
    float getSpeed() const;

    // Door Configuration and Limits
    float getDoorAngleClosedLimit() const;
    float getDoorAngleOpenLimit() const;

    // Detection State Queries
    bool isDetectionDone() const;
    float getDetectedClosedAngle() const;
    float getDetectedOpenAngle() const;

    // Testing modes
    void setEndpointDetectionEnabled(bool enabled);
    void setTorqueSimulationEnabled(bool enabled);
};
