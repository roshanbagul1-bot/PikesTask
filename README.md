# ROS2 MQTT Bridge and ESP32 Firmware Deployment Guide



## Overview



This project demonstrates a complete ROS 2–ESP32 communication pipeline using MQTT.

The system allows a user to control a simulated smart servo from the keyboard in ROS 2. Commands are transmitted through MQTT to an ESP32 (Wokwi simulation), where the servo controller updates the servo state and publishes telemetry back to ROS 2.

The project also simulates:

- Continuous servo rotation
- Gear transmission between motor and door mechanism for robot
- Randomized endpoints
- OLED display for endpoint visualization
- Endpoint detection logic
- Simulated torque for endpoint detection


### ROS 2 Packages

- `mqtt_ros_bridge`

- `mqtt_ros_bridge_msgs`



### ESP32 Firmware

- ESP32 firmware

- Wokwi simulation project



The system uses an ESP32 simulation in Wokwi and ROS 2 packages to exchange data through MQTT.



---



# Repository Structure



```text
repository/
├── ros2_packages/
│   ├── mqtt_ros_bridge/
│   └── mqtt_ros_bridge_msgs/
├── esp32_firmware/
│   ├── firmware_src/          (SmartServo, SmartServoController, MqttSmartServoBridge, ISmartServo)
|   ├── wokwi_project/         (diagram.json, sketch.ino)
│   └── WokWi_ProjectZip/         
├── videos/
├── system_diagram/
└── README.md
```



---
# Features

- ROS2 ↔ MQTT communication
- ESP32 firmware running inside Wokwi
- Keyboard servo control
- Adjustable servo speed
- Clockwise / Anticlockwise rotation
- Gear transmission simulation
- Continuous motor rotation
- Randomized door endpoints
- Endpoint detection
- Simulated torque threshold
- OLED display showing endpoint information



# System Requirements

## Operating System

Ubuntu 22.04 LTS

## ROS

ROS2 Humble

## Software

- Git
- Python3
- Colcon
- rosdep

Python package

```
pip3 install paho-mqtt
```


---



# Deployment



## ROS2 Package Deployment



### Step 1: Obtain the Source Code



#### Option 1 – Clone Repository



```bash

mkdir -p ~/ros2_ws/src

cd ~/ros2_ws/src

git clone <REPOSITORY_LINK>

```



#### Option 2 – Download ZIP



1. Open the GitHub repository.

2. Click **Code → Download ZIP**.

3. Extract the ZIP file.

4. Copy the ROS packages into your ROS workspace:



```text

~/ros2_ws/src/

├── mqtt_ros_bridge/

└── mqtt_ros_bridge_msgs/

```



---



### Step 2: Install Dependencies



Navigate to the workspace root:



```bash

cd ~/ros2_ws

```







Install MQTT Python dependency if required:



```bash

pip3 install paho-mqtt

```



---



### Step 3: Build the Packages



```bash

cd ~/ros2_ws

colcon build 

```



---



### Step 4: Source the Workspace



```bash

source /opt/ros/humble/setup.bash

source ~/ros2_ws/install/setup.bash

```



---



### Step 5: Verify Installation



Verify that both packages were built successfully:



```bash

ros2 pkg list | grep mqtt_ros_bridge

```



Expected output:



```text

mqtt_ros_bridge

mqtt_ros_bridge_msgs

```



---



## ESP32 Firmware Deployment



### Option 1 – Run Using Wokwi Link (Recommended)



Open the following link and start the simulation:



```text

https://wokwi.com/projects/468835726391820289

```



---



### Option 2 – Import Wokwi Project from GitHub



1. Download the repository ZIP from GitHub.

2. Extract the repository on your computer.

3. Open https://wokwi.com

4. Create a new ESP32 project.

5. Replace the generated files with the files located in:



```text

wokwi/

├── diagram.json

└── sketch.ino

```

6. Upload files others files manually.

7. Add libraries from Library Manager(ESP32Servo,PubSubClient,Adafruit SSD1306,Adafruit GFX Library,Onewire)

8. Save the project.

9. Verify Compliance



---



# How to Start the System






## Start ESP32 Simulation

Click **Start Simulation**.

## Start ROS2 Packages

Open a terminal:



```bash

source /opt/ros/humble/setup.bash

source ~/ros2_ws/install/setup.bash

```



Start the ROS2 bridge:



```bash

ros2 run mqtt_ros_bridge mqtt_ros_bridge

ros2 run mqtt_ros_bridge keyboard_control


```
## Verify System Start
As demonstrated in the attached videos, the values displayed in Wokwi will reflect accurately on the ROS terminal.

## Keyboard Controls

| Key | Function |
|------|----------|
| → | Rotate Clockwise |
| ← | Rotate Anticlockwise |
| + | Increase Speed |
| - | Decrease Speed |
| Space | Stop Servo |
| Ctrl+C | Exit Keyboard Node |

---



# Troubleshooting



### Package Not Found



```bash

source /opt/ros/humble/setup.bash

source ~/ros2_ws/install/setup.bash

```



### Rebuild Workspace



```bash

cd ~/ros2_ws

rm -rf build install log

colcon build 

```



---



# Author



**Roshan Sanjay Bagul**
