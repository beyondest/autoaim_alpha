// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from autoaim_interface:msg/CarPos.idl
// generated code does not contain a copyright notice

#ifndef AUTOAIM_INTERFACE__MSG__CAR_POS__STRUCT_H_
#define AUTOAIM_INTERFACE__MSG__CAR_POS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'armor_name'
#include "rosidl_generator_c/string.h"
// Member 'pose'
#include "geometry_msgs/msg/pose_stamped__struct.h"
// Member 'twist'
#include "geometry_msgs/msg/twist_stamped__struct.h"

// Struct defined in msg/CarPos in the package autoaim_interface.
typedef struct autoaim_interface__msg__CarPos
{
  rosidl_generator_c__String armor_name;
  double confidence;
  geometry_msgs__msg__PoseStamped pose;
  geometry_msgs__msg__TwistStamped twist;
} autoaim_interface__msg__CarPos;

// Struct for a sequence of autoaim_interface__msg__CarPos.
typedef struct autoaim_interface__msg__CarPos__Sequence
{
  autoaim_interface__msg__CarPos * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} autoaim_interface__msg__CarPos__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // AUTOAIM_INTERFACE__MSG__CAR_POS__STRUCT_H_
