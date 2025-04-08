#include "motor_controller.h"
#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <geometry_msgs/msg/twist.h>

// -------------------------------------------------------------------
// Integrated Motor Control Code (modified to treat each side as a group)
// -------------------------------------------------------------------

// MotorDriver class (as defined in your integrated library code)
enum MotorMode { PWM_DIR, PWM_PWM };

class MotorDriver {
  public:
    MotorDriver(MotorMode mode, uint8_t pin1, uint8_t pin2) : _mode(mode), _pin1(pin1), _pin2(pin2) {
      pinMode(_pin1, OUTPUT);
      pinMode(_pin2, OUTPUT);
      digitalWrite(_pin1, LOW);
      digitalWrite(_pin2, LOW);
    }
    void setSpeed(int16_t speed) {
      if (speed > 255) speed = 255;
      else if (speed < -255) speed = -255;
      
      if (_mode == PWM_DIR) {
        if (speed >= 0) {
          analogWrite(_pin1, speed);
          digitalWrite(_pin2, LOW);
        } else {
          analogWrite(_pin1, -speed);
          digitalWrite(_pin2, HIGH);
        }
      } else if (_mode == PWM_PWM) {
        if (speed >= 0) {
          analogWrite(_pin1, speed);
          analogWrite(_pin2, 0);
        } else {
          analogWrite(_pin1, 0);
          analogWrite(_pin2, -speed);
        }
      }
    }
  private:
    MotorMode _mode;
    uint8_t _pin1, _pin2;
};

// -------------------------------------------------------------------
// Grouping two motors as one unit.
// -------------------------------------------------------------------
class MotorGroup {
  public:
    MotorGroup(MotorDriver* motorA, MotorDriver* motorB) : _motorA(motorA), _motorB(motorB) { }
    void setSpeed(int16_t speed) {
      _motorA->setSpeed(speed);
      _motorB->setSpeed(speed);
    }
  private:
    MotorDriver* _motorA;
    MotorDriver* _motorB;
};

// -------------------------------------------------------------------
// Instantiate individual motor drivers with your wiring:
//   Front Right Motor: PWM on GP9, Dir on GP8  
//   Front Left  Motor: PWM on GP11, Dir on GP10  
//   Back Right Motor: PWM on GP12, Dir on GP13  
//   Back Left  Motor: PWM on GP14, Dir on GP15
// -------------------------------------------------------------------
MotorDriver motor_front_right(PWM_DIR, 9, 8);
MotorDriver motor_back_right(PWM_DIR, 12, 13);
MotorDriver motor_front_left(PWM_DIR, 11, 10);
MotorDriver motor_back_left(PWM_DIR, 14, 15);

// Create motor groups for left and right sides.
static MotorGroup leftGroup(&motor_front_left, &motor_back_left);
static MotorGroup rightGroup(&motor_front_right, &motor_back_right);

// Global variables for storing latest cmd_vel data.
volatile float g_linear_x = 0.0f;
volatile float g_angular_z = 0.0f;
const float LINEAR_SCALE = 255.0f; // Adjust scaling for your robot

// cmd_vel callback: compute motor speed commands and update both motor groups.
void cmdVelCallback(const void *msgin) {
  const geometry_msgs__msg__Twist* msg = (const geometry_msgs__msg__Twist*)msgin;
  g_linear_x = msg->linear.x;
  g_angular_z = msg->angular.z;
  
  // Compute speeds using a differential drive model.
  float left_speed  = g_linear_x - g_angular_z;
  float right_speed = g_linear_x + g_angular_z;
  
  int left_pwm  = (int)(left_speed  * LINEAR_SCALE);
  int right_pwm = (int)(right_speed * LINEAR_SCALE);
  
  left_pwm  = constrain(left_pwm,  -255, 255);
  right_pwm = constrain(right_pwm, -255, 255);
  
  // Set the grouped motors.
  leftGroup.setSpeed(left_pwm);
  rightGroup.setSpeed(right_pwm);
}

// Global subscription and message variable for cmd_vel.
static rcl_subscription_t cmd_vel_sub;
static geometry_msgs__msg__Twist twist_msg;

void initMotorController(rcl_node_t* node) {
  // Initialize the cmd_vel subscriber on topic "cmd_vel".
  rclc_subscription_init_default(&cmd_vel_sub, node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist), "cmd_vel");
}

void addMotorControllerToExecutor(rclc_executor_t * executor) {
  // Register the cmd_vel subscription callback.
  rclc_executor_add_subscription(executor, &cmd_vel_sub, &twist_msg, &cmdVelCallback, ON_NEW_DATA);
}
