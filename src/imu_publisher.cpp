#include "imu_publisher.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ICM20948.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <sensor_msgs/msg/imu.h>

/// Create an instance of the ICM20948 IMU.
Adafruit_ICM20948 icm;

// Global publisher and message structure.
static rcl_publisher_t imu_pub;
static sensor_msgs__msg__Imu imu_msg;

void initIMUPublisher(rcl_node_t* node) {
  // Attempt to initialize the IMU via I2C.
  if (!icm.begin_I2C()) {
    Serial.println("Failed to find ICM20948 chip");
  } else {
    Serial.println("ICM20948 Found!");
  }
  
  // Initialize the IMU publisher on topic "imu" using Best-Effort QoS.
  rclc_publisher_init_best_effort(&imu_pub, node, 
    ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu), "imu");
}

void publishIMU(void) {
  sensors_event_t accel, gyro, temp, mag;
  icm.getEvent(&accel, &gyro, &temp, &mag);
  
  const char* frame_id_str = "imu_link";
  size_t frame_len = strlen(frame_id_str);
  imu_msg.header.frame_id.data = (char*)frame_id_str;
  imu_msg.header.frame_id.size = frame_len;
  imu_msg.header.frame_id.capacity = frame_len + 1;
  
  // Set orientation (for this example, not computed).
  imu_msg.orientation.x = 0.0f;
  imu_msg.orientation.y = 0.0f;
  imu_msg.orientation.z = 0.0f;
  imu_msg.orientation.w = 1.0f;
  
  // Fill in the angular velocity.
  imu_msg.angular_velocity.x = gyro.gyro.x;
  imu_msg.angular_velocity.y = gyro.gyro.y;
  imu_msg.angular_velocity.z = gyro.gyro.z;
  
  // Fill in the linear acceleration.
  imu_msg.linear_acceleration.x = accel.acceleration.x;
  imu_msg.linear_acceleration.y = accel.acceleration.y;
  imu_msg.linear_acceleration.z = accel.acceleration.z;
  
  rcl_publish(&imu_pub, &imu_msg, NULL);
}
