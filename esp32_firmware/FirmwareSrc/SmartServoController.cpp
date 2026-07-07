#include "SmartServoController.h"
#include <algorithm>

// Constructor
SmartServoController::SmartServoController(ISmartServo* servo)
    : smartServo(servo) {
}

// Lifecycle and Run Loop
void SmartServoController::run(float dt) {
    if (!smartServo) return;

    if (endpointDetectionEnabled && detectionState != DetectionState::DONE) {
        runEndpointDetection(dt);
        return;
    }

    smartServo->update(dt);   // now blocked correctly at the detected limits
}

// Commands and Directions
void SmartServoController::setCommand(float cmd) {
    if (smartServo && detectionState == DetectionState::DONE) {
        smartServo->setCommand(cmd);
    }
    // Ignored during detection: detection owns the command until DONE.
}

void SmartServoController::setCommandDirection(bool clockwise) {
    if (smartServo && detectionState == DetectionState::DONE) {
        smartServo->setCommandDirection(clockwise);
    }
}

bool SmartServoController::getCommandedDirection() const {
    return smartServo ? smartServo->getCommandedDirection() : true;
}

void SmartServoController::setEndpointDetectionEnabled(bool enabled) {
    endpointDetectionEnabled = enabled;
    if (!enabled) {
        detectionState = DetectionState::DONE;
    } else {
        detectionState = DetectionState::SEEK_OPEN;
    }
}

void SmartServoController::setTorqueSimulationEnabled(bool enabled) {
    if (smartServo) {
        smartServo->setTorqueSimulation(enabled);
    }
}

// Motion and Feedback State
float SmartServoController::getMotorAngle() const {
    // Return the current motor angle, or zero if no servo is attached.
    if (smartServo) {
        return smartServo->getMotorAngle();
    }
    return 0.0f;
}

float SmartServoController::getDoorAngle() const {
    // Return the door angle, or zero if no servo is attached.
    if (smartServo) {
        return smartServo->getDoorAngle();
    }
    return 0.0f;
}

float SmartServoController::getSpeed() const {
    // Return the current speed, or zero if no servo is attached.
    if (smartServo) {
        return smartServo->getSpeed();
    }
    return 0.0f;
}

// Door Configuration and Limits
float SmartServoController::getDoorAngleClosedLimit() const {
    // Return the door angle, or zero if no servo is attached.
    if (smartServo) {
        return smartServo->getDoorAngleClosedLimit();
    }
    return 0.0f;
}

float SmartServoController::getDoorAngleOpenLimit() const {
    // Return the door angle, or zero if no servo is attached.
    if (smartServo) {
        return smartServo->getDoorAngleOpenLimit();
    }
    return 0.0f;
}

// Detection State Queries
bool SmartServoController::isDetectionDone() const {
    return detectionState == DetectionState::DONE;
}

float SmartServoController::getDetectedClosedAngle() const { 
    return detectedClosedAngle; 
}

float SmartServoController::getDetectedOpenAngle() const { 
    return detectedOpenAngle; 
}

// Private Detection Routine
void SmartServoController::runEndpointDetection(float dt) {
    if (!smartServo) return;

    switch (detectionState) {
        case DetectionState::SEEK_OPEN:
            smartServo->setCommand(DETECTION_SPEED);
            smartServo->setCommandDirection(true); // toward open
            smartServo->update(dt);

            if (smartServo->getSimulatedTorque() > TORQUE_THRESHOLD) {
                detectedOpenAngle = smartServo->getDoorAngle();
                smartServo->setCommand(0.0f);
                detectionState = DetectionState::SEEK_CLOSE;
                
            }
            break;

        case DetectionState::SEEK_CLOSE:
            smartServo->setCommand(DETECTION_SPEED);
            smartServo->setCommandDirection(false); // toward closed
            smartServo->update(dt);

            if (smartServo->getSimulatedTorque() > TORQUE_THRESHOLD) {
                detectedClosedAngle = smartServo->getDoorAngle();
                smartServo->setCommand(0.0f);
                // Hand the detected limits down so update() enforces them directly.
                smartServo->applyDetectedLimits(detectedClosedAngle, detectedOpenAngle);
                detectionState = DetectionState::DONE;
            }
            break;

        case DetectionState::DONE:
            // Hand the detected limits down so update() enforces them directly.
            smartServo->applyDetectedLimits(detectedClosedAngle, detectedOpenAngle);
            break; 
    }
}
