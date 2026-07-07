#pragma once
#include "ISmartServo.h"
#include <ESP32Servo.h>

class SmartServo : public ISmartServo {
private:
    Servo hwServo;
    int pin;
    float motorAngle;
    float command;
    float transmissionRatio;
    bool commandedDirection;

    // Endpoint Detection State
    float doorAngleOpenLimit = 0.0f;
    float doorAngleClosedLimit = 0.0f;
    float simulatedTorque = 0.0f;
    bool enforceEndpoints = true;
    bool simulateTorque = true;
    
    static constexpr float TORQUE_GAIN = 2.0f;
    static constexpr float TORQUE_DECAY = 5.0f;

public:
    SmartServo(int servoPin, float transmissionRatio = 15.0f / 40.0f);
    virtual ~SmartServo() override = default;

    // Lifecycle and Updates
    void begin() override;
    void update(float dt) override;

    // Commands and Directions
    void setCommand(float cmd) override;
    void setCommandDirection(bool clockwise) override; 
    bool getCommandedDirection() const override;        

    // Motion and Feedback State
    float getMotorAngle() const override;
    float getSpeed() const override;
    float getSimulatedTorque() const override;

    // Door Configuration and Limits
    void applyDetectedLimits(float closedAngle, float openAngle) override; 
    float getDoorAngle() const override;
    float getDoorAngleClosedLimit() const override;
    float getDoorAngleOpenLimit() const override;

    // Simulation
    void generateRandomEndpoints();

    // Testing modes
    void setEndpointEnforcement(bool enforce);
    void setTorqueSimulation(bool simulate);
};
