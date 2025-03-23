#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>
#include <sensor_msgs/msg/imu.h>

#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20948.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_ICM20948 icm;

// Global micro-ROS objects
rcl_node_t node;
rclc_executor_t executor;
static rclc_support_t support;
static rcl_allocator_t allocator;

// Publishers
static rcl_publisher_t imu_pub;
static sensor_msgs__msg__Imu imu_msg;


bool initMicroROS(){

  set_microros_serial_transports(Serial);

  allocator = rcl_get_default_allocator();
  rclc_support_init(&support, 0, NULL, &allocator);

  // Create node
  rcl_ret_t rc = rclc_node_init_default(
    &node,
    "cytron2350",
    "",
    &support
  );
  if (rc != RCL_RET_OK) {
    Serial.println("Error creating micro-ROS node!");
    return false;
  }

  // Create executor
  rclc_executor_init(&executor, &support.context, 1, &allocator);

    // Initialize IMU Publisher with Best-Effort QoS
  rclc_publisher_init_best_effort(
    &imu_pub,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
    "imu"
  );

  return true;

}

void setup() {
  // Initialize serial for debug output.
  Serial.begin(115200);
  while (!Serial) { }
  initMicroROS();



  Wire.setSCL(17);
  Wire.setSDA(16);
  Wire.begin();
  if (!icm.begin_I2C()) {
    // if (!icm.begin_SPI(ICM_CS)) {
    // if (!icm.begin_SPI(ICM_CS, ICM_SCK, ICM_MISO, ICM_MOSI)) {

    Serial.println("Failed to find ICM20948 chip");
  }
  Serial.println("ICM20948 Found!");
  // icm.setAccelRange(ICM20948_ACCEL_RANGE_16_G);

}

void publishIMU()
{
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t mag;
  sensors_event_t temp;
  icm.getEvent(&accel, &gyro, &temp, &mag);

  const char* frame_id_str = "imu_link";
  size_t frame_len = strlen(frame_id_str);

  imu_msg.header.frame_id.data = (char*)frame_id_str;
  imu_msg.header.frame_id.size = frame_len;
  imu_msg.header.frame_id.capacity = frame_len + 1;

  imu_msg.orientation.x = accel.acceleration.heading;
  imu_msg.orientation.y = accel.acceleration.pitch;
  imu_msg.orientation.z = accel.acceleration.roll;
  imu_msg.orientation.w = 1.0f;

  imu_msg.angular_velocity.x = gyro.gyro.x;
  imu_msg.angular_velocity.y = gyro.gyro.y;
  imu_msg.angular_velocity.z = gyro.gyro.z;

  imu_msg.linear_acceleration.x = accel.acceleration.x;
  imu_msg.linear_acceleration.x = accel.acceleration.y;
  imu_msg.linear_acceleration.x = accel.acceleration.z;


  rcl_publish(&imu_pub, &imu_msg, NULL);
}

void loop() {

   publishIMU();


   rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
}