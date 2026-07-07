#include "SmartServo.h"

namespace {
    // Commented out unused utility function
    /*
    float servoEndValues(float value, float minValue, float maxValue) {
        if (value < minValue) {
            return minValue;
        }
        if (value > maxValue) {
            return maxValue;
        }
        return value;
    }
    */
}

// Constructor
SmartServo::SmartServo(int servoPin, float transmissionRatio)
    : pin(servoPin),
      motorAngle(0.0f),
      command(0.0f),
      transmissionRatio(transmissionRatio),
      commandedDirection(true)   {
}

// Lifecycle and Updates
void SmartServo::begin() {
    hwServo.setPeriodHertz(50);
    hwServo.attach(pin, 500, 2400);
    hwServo.write(90);
}

void SmartServo::update(float dt) {
    if (dt < 0.0f) dt = 0.0f;

    float directionMultiplier = commandedDirection ? 1.0f : -1.0f;
    float proposedMotorAngle = motorAngle + (command * directionMultiplier * 180.0f * dt);
    float proposedDoorAngle = proposedMotorAngle * transmissionRatio;

    // `motorAngle` is the simulated internal rotation amount.
    // When endpoint enforcement is disabled, it can advance continuously.
    if (!enforceEndpoints) {
        motorAngle = proposedMotorAngle;
    } else {
        bool blockedAtOpen  = commandedDirection  && proposedDoorAngle >= doorAngleOpenLimit;
        bool blockedAtClose = !commandedDirection && proposedDoorAngle <= doorAngleClosedLimit;

        if (blockedAtOpen) {
            if (simulateTorque) {
                simulatedTorque += fabs(command) * dt * TORQUE_GAIN;
            }
            motorAngle = doorAngleOpenLimit / transmissionRatio;
        } 
        else if (blockedAtClose) {
            if (simulateTorque) {
                simulatedTorque += fabs(command) * dt * TORQUE_GAIN;
            }
            motorAngle = doorAngleClosedLimit  / transmissionRatio;
        } 
        else {
            motorAngle = proposedMotorAngle;
            if (simulateTorque) {
                simulatedTorque = max(0.0f, simulatedTorque - TORQUE_DECAY * dt);
            }
        }
    }

    if (motorAngle >= 360.0f) motorAngle -= 360.0f;
    if (motorAngle < 0.0f) motorAngle += 360.0f;

    // Convert internal motor angle into the actual servo output range.
    // The hardware servo only supports 0..180 degrees, so we mirror values
    // above 180 back into the valid range for display and hardware output.
    float servoAngle = fmod(motorAngle * transmissionRatio, 360.0f);
    if (servoAngle > 180.0f) servoAngle = 360.0f - servoAngle;
    hwServo.write(servoAngle);
}

// Commands and Directions
void SmartServo::setCommand(float cmd) {
    command = cmd;
}

void SmartServo::setCommandDirection(bool clockwise) {
    commandedDirection = clockwise;
}

bool SmartServo::getCommandedDirection() const {
    return commandedDirection;
}

// Motion and Feedback State
float SmartServo::getMotorAngle() const {
    return motorAngle;
}

float SmartServo::getSpeed() const {
    // Return speed as a positive value, regardless of direction.
    if (command < 0.0f) {
        return -command;
    }
    return command;
}

float SmartServo::getSimulatedTorque() const {
    return simulatedTorque;
}

// Door Configuration and Limits
void SmartServo::applyDetectedLimits(float closedAngle, float openAngle) {
    doorAngleClosedLimit = (closedAngle < openAngle) ? closedAngle : openAngle;
    doorAngleOpenLimit   = (closedAngle < openAngle) ? openAngle : closedAngle;
}

float SmartServo::getDoorAngle() const {
    return motorAngle * transmissionRatio;
}

float SmartServo::getDoorAngleClosedLimit() const { 
    return doorAngleClosedLimit; 
}

float SmartServo::getDoorAngleOpenLimit() const { 
    return doorAngleOpenLimit; 
}

// Simulation 
void SmartServo::generateRandomEndpoints() {
    float centerDoor = random(30, 150);

    doorAngleClosedLimit = centerDoor - 30;
    doorAngleOpenLimit   = centerDoor + 30;

    // safety clamp for servo limits
    if (doorAngleClosedLimit < 0) doorAngleClosedLimit = 0;
    if (doorAngleOpenLimit > 180) doorAngleOpenLimit = 180;

    //endpointsGenerated = true;
}

void SmartServo::setEndpointEnforcement(bool enforce) {
    enforceEndpoints = enforce;
}

void SmartServo::setTorqueSimulation(bool simulate) {
    simulateTorque = simulate;
    if (!simulateTorque) {
        simulatedTorque = 0.0f;
    }
}
