#pragma once

class ISmartServo {
public:
    virtual ~ISmartServo() {}

    // Lifecycle and Updates
    virtual void begin() = 0;
    virtual void update(float dt) = 0;

    // Commands and Directions
    virtual void setCommand(float cmd) = 0;
    virtual void setCommandDirection(bool clockwise) = 0;
    virtual bool getCommandedDirection() const = 0;

    // Motion and Feedback State
    virtual float getMotorAngle() const = 0;
    virtual float getSpeed() const = 0;
    virtual float getSimulatedTorque() const = 0;

    // Door Configuration and Limits
    virtual void applyDetectedLimits(float closedAngle, float openAngle) = 0;
    virtual float getDoorAngle() const = 0;
    virtual float getDoorAngleClosedLimit() const = 0;
    virtual float getDoorAngleOpenLimit() const = 0;

    virtual void setTorqueSimulation(bool enabled) = 0;


};
