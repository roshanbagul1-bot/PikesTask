#!/usr/bin/env python3

import sys
import select
import termios
import tty

import rclpy
from rclpy.node import Node

from mqtt_ros_bridge_msgs.msg import ServoCommand


class KeyboardControl(Node):

    def __init__(self):
        super().__init__("keyboard_control")

        # Save terminal settings so they can be restored on exit.
        self.terminal_file_descriptor = sys.stdin.fileno()
        self.original_terminal_settings = termios.tcgetattr(self.terminal_file_descriptor)

        # Publisher used to send servo commands.
        self.servo_command_publisher = self.create_publisher(
            ServoCommand,
            "ROS_To_ESP/cmd",
            10,
        )

        # Initial servo state.
        self.motor_speed = 0.3
        self.is_clockwise_direction = True      # True = Clockwise, False = Anticlockwise

        self.get_logger().info("Keyboard control started")

    def get_key(self):
        """
        Read one key without waiting forever.
        Returns None if no key was pressed.
        """
        tty.setcbreak(self.terminal_file_descriptor)

        is_data_ready_to_read, _, _ = select.select([sys.stdin], [], [], 0.1)

        if is_data_ready_to_read:
            return sys.stdin.read(1)

        return None

    def publish_command(self):
        """Publish the current speed and direction."""

        servo_command_msg = ServoCommand()
        servo_command_msg.cmd = self.motor_speed
        servo_command_msg.direction = self.is_clockwise_direction

        self.servo_command_publisher.publish(servo_command_msg)

        self.get_logger().info(
            f"CMD -> speed={servo_command_msg.cmd:.2f}"
        )
        self.get_logger().info(
            f"dir={'CW' if servo_command_msg.direction else 'CCW'}"
        )

    def restore_terminal(self):
        """Restore the terminal before exiting."""
        termios.tcsetattr(
            self.terminal_file_descriptor,
            termios.TCSADRAIN,
            self.original_terminal_settings,
        )

    def run(self):
        """
        Main keyboard loop.

        Controls:
          Right Arrow : Clockwise
          Left Arrow  : Anticlockwise
          +           : Increase speed
          -           : Decrease speed
          Space       : Stop servo
          Ctrl+C      : Exit
        """
        try:
            while rclpy.ok():

                received_character = self.get_key()

                if received_character is None:
                    continue

                # Ctrl+C
                if received_character == "\x03":
                    break

                # Arrow keys are sent as ESC [ C/D escape sequences
                if received_character == "\x1b":

                    _ignored_bracket = sys.stdin.read(1)      # Skip '['
                    arrow_key_code = sys.stdin.read(1)

                    if arrow_key_code == "C":
                        self.is_clockwise_direction = True

                    elif arrow_key_code == "D":
                        self.is_clockwise_direction = False

                elif received_character == "+":
                    self.motor_speed = min(1.0, self.motor_speed + 0.1)

                elif received_character == "-":
                    self.motor_speed = max(0.0, self.motor_speed - 0.1)

                elif received_character == " ":
                    self.motor_speed = 0.0

                self.publish_command()

        finally:
            self.restore_terminal()


def main():
    rclpy.init()

    keyboard_control_node = KeyboardControl()

    try:
        keyboard_control_node.run()

    except KeyboardInterrupt:
        pass

    finally:
        keyboard_control_node.restore_terminal()
        keyboard_control_node.get_logger().info("Keyboard control stopped.")

        keyboard_control_node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
