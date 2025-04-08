#include "odom_publisher.h"
#include "encoder.h"   // Provides getRightEncoderTicks() and getLeftEncoderTicks()
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <nav_msgs/msg/odometry.h>
#include <Arduino.h>
#include <string.h>

// Global publisher and odometry message.
static rcl_publisher_t odom_pub;
static nav_msgs__msg__Odometry odom_msg;

// Physical and encoder parameters – adjust to your hardware.
const float WHEEL_RADIUS = 0.03;    // meters
const int TICKS_PER_REV = 100;      // ticks per revolution
const float WHEEL_BASE = 0.15;      // meters (distance between wheels)

static long last_right_ticks = 0;
static long last_left_ticks = 0;
static float x_pose = 0.0, y_pose = 0.0, theta_pose = 0.0;

void initOdomPublisher(rcl_node_t* node) {
  // Initialize the odometry publisher on the "odom" topic.
  rclc_publisher_init_best_effort(&odom_pub, node, 
    ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry), "odom");
}

void publishOdom(void) {
  // Retrieve current encoder counts via the encoder module.
  long current_right = getRightEncoderTicks();
  long current_left  = getLeftEncoderTicks();
  
  // Calculate differences (delta ticks) since the last update.
  long delta_right = current_right - last_right_ticks;
  long delta_left  = current_left - last_left_ticks;
  
  // Update last tick counts.
  last_right_ticks = current_right;
  last_left_ticks = current_left;
  
  // Convert tick differences to traveled distance (wheel circumference).
  float dist_right = (2 * PI * WHEEL_RADIUS) * ((float)delta_right / TICKS_PER_REV);
  float dist_left  = (2 * PI * WHEEL_RADIUS) * ((float)delta_left  / TICKS_PER_REV);
  
  // Compute average distance and change in heading.
  float delta_dist = (dist_right + dist_left) / 2.0;
  float delta_theta = (dist_right - dist_left) / WHEEL_BASE;
  
  // Update pose (using a simple dead-reckoning model).
  x_pose += delta_dist * cos(theta_pose + (delta_theta / 2.0));
  y_pose += delta_dist * sin(theta_pose + (delta_theta / 2.0));
  theta_pose += delta_theta;
  
  // Update header timestamp (here using millis() for simplicity).
  uint64_t now_ns = ((uint64_t)millis()) * 1000000ULL;
  odom_msg.header.stamp.sec = now_ns / 1000000000;
  odom_msg.header.stamp.nanosec = now_ns % 1000000000;
  
  // Initialize header strings similarly to the IMU message.
  const char* header_frame = "odom";
  size_t header_len = strlen(header_frame);
  odom_msg.header.frame_id.data = (char*)header_frame;
  odom_msg.header.frame_id.size = header_len;
  odom_msg.header.frame_id.capacity = header_len + 1;
  
  const char* child_frame = "base_link";
  size_t child_len = strlen(child_frame);
  odom_msg.child_frame_id.data = (char*)child_frame;
  odom_msg.child_frame_id.size = child_len;
  odom_msg.child_frame_id.capacity = child_len + 1;
  
  // Populate pose.
  odom_msg.pose.pose.position.x = x_pose;
  odom_msg.pose.pose.position.y = y_pose;
  odom_msg.pose.pose.position.z = 0.0;
  
  // Convert heading (theta) to quaternion.
  float half_theta = theta_pose / 2.0;
  odom_msg.pose.pose.orientation.x = 0.0;
  odom_msg.pose.pose.orientation.y = 0.0;
  odom_msg.pose.pose.orientation.z = sin(half_theta);
  odom_msg.pose.pose.orientation.w = cos(half_theta);
  
  // Optionally, set twist (here the incremental changes).
  odom_msg.twist.twist.linear.x = delta_dist;
  odom_msg.twist.twist.angular.z = delta_theta;
  
  rcl_publish(&odom_pub, &odom_msg, NULL);
}
