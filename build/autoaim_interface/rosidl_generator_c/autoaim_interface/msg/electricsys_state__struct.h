// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from autoaim_interface:msg/ElectricsysState.idl
// generated code does not contain a copyright notice

#ifndef AUTOAIM_INTERFACE__MSG__ELECTRICSYS_STATE__STRUCT_H_
#define AUTOAIM_INTERFACE__MSG__ELECTRICSYS_STATE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'nullbyte'
#include "rosidl_generator_c/string.h"

// Struct defined in msg/ElectricsysState in the package autoaim_interface.
typedef struct autoaim_interface__msg__ElectricsysState
{
  double cur_yaw;
  double cur_pitch;
  double unix_time;
  int64_t sentry_health;
  rosidl_generator_c__String nullbyte;
} autoaim_interface__msg__ElectricsysState;

// Struct for a sequence of autoaim_interface__msg__ElectricsysState.
typedef struct autoaim_interface__msg__ElectricsysState__Sequence
{
  autoaim_interface__msg__ElectricsysState * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} autoaim_interface__msg__ElectricsysState__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // AUTOAIM_INTERFACE__MSG__ELECTRICSYS_STATE__STRUCT_H_
