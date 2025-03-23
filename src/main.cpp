#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <std_msgs/msg/int32.h>

rcl_publisher_t publisher;
std_msgs__msg__Int32 msg;
rcl_node_t node;
rclc_support_t support;
rcl_allocator_t allocator;

void setup() {
  // Initialize serial for debug output.
  Serial.begin(115200);
  while (!Serial) { }

  // Setup micro-ROS transport (this function is provided by micro_ros_platformio)
  // It configures the serial, UDP, or other transport.
  set_microros_serial_transports(Serial);
  delay(2000);  // Give the transport a moment to initialize

  // Create allocator and support structures.
  allocator = rcl_get_default_allocator();
  rclc_support_init(&support, 0, NULL, &allocator);

  // Create a node with the name "int32_publisher"
  rclc_node_init_default(&node, "int32_publisher", "", &support);

  // Create a publisher for topic "test_int32" of type std_msgs/msg/Int32
  rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "test_int32");

  // Initialize message data.
  msg.data = 0;

  Serial.println("Publisher initialized.");
}

void loop() {
  // Increment the data and publish the message.
  msg.data++;
  if (rcl_publish(&publisher, &msg, NULL) != RCL_RET_OK) {
    Serial.println("Error publishing message");
  } else {
    Serial.print("Published: ");
    Serial.println(msg.data);
  }

  // Wait 1 second between publishes.
  delay(1000);
}
