#ifndef VOLTAGE_SENSOR_H
#define VOLTAGE_SENSOR_H

#include <rcl/rcl.h>

#ifdef __cplusplus
extern "C" {
#endif

void initVoltagePublisher(rcl_node_t* node);
void publishVoltage(void);

#ifdef __cplusplus
}
#endif

#endif // VOLTAGE_SENSOR_H
