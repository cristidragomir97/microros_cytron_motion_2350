#ifndef ODOM_PUBLISHER_H
#define ODOM_PUBLISHER_H

#include <rcl/rcl.h>

#ifdef __cplusplus
extern "C" {
#endif

void initOdomPublisher(rcl_node_t* node);
void publishOdom(void);

#ifdef __cplusplus
}
#endif

#endif // ODOM_PUBLISHER_H
