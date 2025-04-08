#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <rcl/rcl.h>
#include <rclc/executor.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialize the servo subscriber (attaches the servos and sets up the subscription).
void initServoSubscriber(rcl_node_t* node);
// Add the servo subscription to the executor.
void addServoSubscriptionToExecutor(rclc_executor_t * executor);

#ifdef __cplusplus
}
#endif

#endif // SERVO_CONTROL_H
