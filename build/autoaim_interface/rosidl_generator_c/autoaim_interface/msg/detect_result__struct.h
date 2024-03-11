// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from autoaim_interface:msg/DetectResult.idl
// generated code does not contain a copyright notice

#ifndef AUTOAIM_INTERFACE__MSG__DETECT_RESULT__STRUCT_H_
#define AUTOAIM_INTERFACE__MSG__DETECT_RESULT__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'detect_result'
#include "autoaim_interface/msg/each_detect_result__struct.h"

// Struct defined in msg/DetectResult in the package autoaim_interface.
typedef struct autoaim_interface__msg__DetectResult
{
  autoaim_interface__msg__EachDetectResult__Sequence detect_result;
} autoaim_interface__msg__DetectResult;

// Struct for a sequence of autoaim_interface__msg__DetectResult.
typedef struct autoaim_interface__msg__DetectResult__Sequence
{
  autoaim_interface__msg__DetectResult * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} autoaim_interface__msg__DetectResult__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // AUTOAIM_INTERFACE__MSG__DETECT_RESULT__STRUCT_H_
