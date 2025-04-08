#ifndef ENCODER_H
#define ENCODER_H

#include <rcl/rcl.h>
#include "pio_encoder.h"  // pio_encoder library header

#ifdef __cplusplus
extern "C" {
#endif

// Initialize the encoder module and its publishers with the given encoder instances.
void initEncoders(rcl_node_t* node, PioEncoder* rightEncoder, PioEncoder* leftEncoder);

// Returns the current tick count for the right wheel.
long getRightEncoderTicks(void);

// Returns the current tick count for the left wheel.
long getLeftEncoderTicks(void);

// Publishes the raw encoder counts on the topics "/wheel/right" and "/wheel/left".
void publishWheelEncoders(void);

#ifdef __cplusplus
}
#endif

#endif // ENCODER_H
