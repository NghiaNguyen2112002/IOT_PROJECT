#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include <stdint.h>


// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int sensor_node_id;
  float humi;
  float temp;
} struct_message;

// Create a struct_message called myData
extern struct_message myData; 








#endif
