// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from autoaim_interface:msg/ElectricsysCom.idl
// generated code does not contain a copyright notice

#ifndef AUTOAIM_INTERFACE__MSG__ELECTRICSYS_COM__STRUCT_H_
#define AUTOAIM_INTERFACE__MSG__ELECTRICSYS_COM__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'sof'
#include "rosidl_generator_c/string.h"

// Struct defined in msg/ElectricsysCom in the package autoaim_interface.
typedef struct autoaim_interface__msg__ElectricsysCom
{
  rosidl_generator_c__String sof;
  double target_abs_pitch;
  double target_abs_yaw;
  double reach_unix_time;
  int64_t fire_times;
  int64_t reserved_slot;
} autoaim_interface__msg__ElectricsysCom;

// Struct for a sequence of autoaim_interface__msg__ElectricsysCom.
typedef struct autoaim_interface__msg__ElectricsysCom__Sequence
{
  autoaim_interface__msg__ElectricsysCom * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} autoaim_interface__msg__ElectricsysCom__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // AUTOAIM_INTERFACE__MSG__ELECTRICSYS_COM__STRUCT_H_
