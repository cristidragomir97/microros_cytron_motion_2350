#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <rcl/rcl.h>
#include <rclc/executor.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initializes the motor controller (creates a /cmd_vel subscriber).
void initMotorController(rcl_node_t* node);

// Adds the cmd_vel subscription to the executor.
void addMotorControllerToExecutor(rclc_executor_t * executor);

#ifdef __cplusplus
}
#endif

#endif // MOTOR_CONTROLLER_H
