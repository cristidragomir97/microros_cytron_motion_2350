#include "servo_control.h"
#include <Arduino.h>
#include <Servo.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <std_msgs/msg/float32.h>

// Create two Servo objects for the camera tilt.
// One is mounted normally (on GP7) and the other is reversed (on GP6).
Servo servo1;
Servo servo2;

// Global subscriber and message for camera tilt.
static rcl_subscription_t servo_sub;
static std_msgs__msg__Float32 tilt_msg;

// Callback for when a new tilt angle is received.
void servoTiltCallback(const void *msgin) {
  const std_msgs__msg__Float32 * tilt = (const std_msgs__msg__Float32 *)msgin;
  float angle = tilt->data;
  // Write the angle to servo1 and the inverse (back-to-back mounting) to servo2.
  servo1.write(angle);
  servo2.write(180 - angle);
}

void initServoSubscriber(rcl_node_t* node) {
  // Attach servos to pins GP7 and GP6.
  servo1.attach(7);
  servo2.attach(6);
  
  // Initialize the subscriber for the "/camera/tilt" topic.
  rclc_subscription_init_default(&servo_sub, node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32), "camera/tilt");
}

void addServoSubscriptionToExecutor(rclc_executor_t * executor) {
  rclc_executor_add_subscription(executor, &servo_sub, &tilt_msg, &servoTiltCallback, ON_NEW_DATA);
}
