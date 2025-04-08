#ifndef IMU_PUBLISHER_H
#define IMU_PUBLISHER_H

#include <rcl/rcl.h>

#ifdef __cplusplus
extern "C" {
#endif

void initIMUPublisher(rcl_node_t* node);
void publishIMU(void);

#ifdef __cplusplus
}
#endif

#endif // IMU_PUBLISHER_H
