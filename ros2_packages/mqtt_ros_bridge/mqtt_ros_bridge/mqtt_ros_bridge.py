import rclpy
from rclpy.node import Node
from std_msgs.msg import String, Float32
import paho.mqtt.client as mqtt
from mqtt_ros_bridge_msgs.msg import ServoCommand

class MqttRosBridge(Node):

    def __init__(self):
        super().__init__('mqtt_ros_bridge')

        # ROS To MQTT
        self.create_subscription(
            ServoCommand,
            'ROS_To_ESP/cmd',
            self.cmd_callback,
            10
        )

        # MQTT To ROS
        self.pub_state = self.create_publisher(
            String,
            'ESP_To_ROS/state',
            10
        )

        # MQTT client
        self.mqtt = mqtt.Client()
        self.mqtt.on_message = self.on_message

        self.mqtt.connect("test.mosquitto.org", 1883)

        self.mqtt.subscribe("ESP_To_ROS/state")
        self.mqtt.loop_start()

        self.get_logger().info("MQTT ROS Bridge Started")

    # ROS To MQTT
    def cmd_callback(self, msg):
        self.get_logger().info(
            f"ROS To MQTT cmd={msg.cmd}, dir={msg.direction}"
        )

        payload = f"{msg.cmd},{int(msg.direction)}"
        self.mqtt.publish("ROS_To_ESP/cmd", payload)

    # MQTT To ROS
    def on_message(self, client, userdata, msg):
        # Decode the raw payload string
        payload_str = msg.payload.decode()

        # Publish the raw string to ROS as requested by your architecture
        ros_msg = String()
        ros_msg.data = payload_str
        self.pub_state.publish(ros_msg)

        try:
            # Split the string by commas (e.g., "45,90,12.5,1")
            # Order assumed: motor_angle, door_angle, speed, direction
            motor, door, speed, direction = payload_str.split(",")

            # Log with clean, inline labels
            self.get_logger().info(
                f"MQTT To ROS To Motor: {motor}°, Door: {door}°, Speed: {speed}, Dir: {direction}"
            )
            
        except ValueError:
            # Fallback log if the payload string format is unexpected
            self.get_logger().warn(f"MQTT To ROS (Raw Data Format): {payload_str}")


def main():
    rclpy.init()
    node = MqttRosBridge()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()