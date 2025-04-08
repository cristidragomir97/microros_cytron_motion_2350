#include <Arduino.h>
#include <Wire.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

// Include module headers
#include "imu_publisher.h"
#include "motor_controller.h"
#include "encoder.h"
#include "odom_publisher.h"
#include "voltage_sensor.h"
#include "servo_control.h"

// Include the pio_encoder library header.
#include <pio_encoder.h>

// Create global encoder instances using a single instance per wheel.
// (Right wheel uses pins 2 (channel A) and 6 (channel B),
//  Left wheel uses pins 8 (channel A) and 22 (channel B).)
PioEncoder encoder_right(0, 1);
PioEncoder encoder_left(3, 4);

// Global micro-ROS objects.
rcl_node_t node;
rclc_support_t support;
rcl_allocator_t allocator;
rclc_executor_t executor;

void initMicroROS() {
  set_microros_serial_transports(Serial);
  allocator = rcl_get_default_allocator();
  rclc_support_init(&support, 0, NULL, &allocator);
  
  rcl_ret_t rc = rclc_node_init_default(&node, "cytron2350", "", &support);
  if (rc != RCL_RET_OK) {
    Serial.println("Error creating micro-ROS node!");
    while (1);
  }
  
  // Initialize executor with room for all subscriptions.
  rclc_executor_init(&executor, &support.context, 10, &allocator);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { }
  Serial.println("Starting Cytron2350 system...");
  
  initMicroROS();
  
  Wire.begin();
  
  // Initialize modules.
  initIMUPublisher(&node);
  initMotorController(&node);
  // Pass the global encoder instances to the encoder module.
  initEncoders(&node, &encoder_right, &encoder_left);
  initOdomPublisher(&node);
  initVoltagePublisher(&node);
  initServoSubscriber(&node);
  
  // Add subscription-based modules to the executor.
  addMotorControllerToExecutor(&executor);
  addServoSubscriptionToExecutor(&executor);
  
  Serial.println("All modules initialized.");
}

void loop() {
  // Publish sensor data.
  publishIMU();
  publishWheelEncoders();
  publishOdom();
  publishVoltage();
  
  // Process subscriptions and callbacks.
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
  
  delay(20);
}
