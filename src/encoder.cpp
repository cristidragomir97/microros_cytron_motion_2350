#include "encoder.h"
#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <std_msgs/msg/int32.h>

// Global publisher objects and messages.
static rcl_publisher_t right_encoder_pub;
static rcl_publisher_t left_encoder_pub;
static std_msgs__msg__Int32 right_encoder_msg;
static std_msgs__msg__Int32 left_encoder_msg;

// Static pointers to the passed-in encoder instances.
static PioEncoder* _encoder_right = nullptr;
static PioEncoder* _encoder_left = nullptr;

void initEncoders(rcl_node_t* node, PioEncoder* rightEncoder, PioEncoder* leftEncoder) {
  // Store the provided encoder instances.
  _encoder_right = rightEncoder;
  _encoder_left = leftEncoder;
  
  // Call begin() on the encoders as required by the library.
  _encoder_right->begin();
  _encoder_left->begin();
  
  // Initialize the encoder publishers.
  rclc_publisher_init_best_effort(&right_encoder_pub, node, 
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32), "wheel/right");
  rclc_publisher_init_best_effort(&left_encoder_pub, node, 
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32), "wheel/left");
}

long getRightEncoderTicks(void) {
  if (_encoder_right) {
    return _encoder_right->getCount();
  }
  return 0;
}

long getLeftEncoderTicks(void) {
  if (_encoder_left) {
    return _encoder_left->getCount();
  }
  return 0;
}

void publishWheelEncoders(void) {
  long rightCount = getRightEncoderTicks();
  long leftCount = getLeftEncoderTicks();
  
  right_encoder_msg.data = rightCount;
  left_encoder_msg.data = leftCount;
  
  rcl_publish(&right_encoder_pub, &right_encoder_msg, NULL);
  rcl_publish(&left_encoder_pub, &left_encoder_msg, NULL);
}
