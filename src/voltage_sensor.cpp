#include "voltage_sensor.h"
#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <std_msgs/msg/float32.h>

// Define the ADC pin for the voltage sensor (GP29/ADC3).
const int VOLTAGE_SENSOR_PIN = 29;

// Global publisher and message.
static rcl_publisher_t voltage_pub;
static std_msgs__msg__Float32 voltage_msg;

void initVoltagePublisher(rcl_node_t* node) {
  rclc_publisher_init_best_effort(&voltage_pub, node, 
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32), "voltage");
}

void publishVoltage(void) {
  // Read the ADC value (0 to 4095 for a 12-bit ADC).
  int adc_value = analogRead(VOLTAGE_SENSOR_PIN);
  float VADC = (adc_value / 4095.0) * 3.3;  // Convert reading to voltage (assuming 3.3V reference)
  // Apply the given formula: VIN = VADC/6.1
  float battery_voltage = VADC * 6.1;
  
  voltage_msg.data = battery_voltage;
  rcl_publish(&voltage_pub, &voltage_msg, NULL);
}
