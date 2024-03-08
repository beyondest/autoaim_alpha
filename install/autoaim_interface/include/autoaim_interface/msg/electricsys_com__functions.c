// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from autoaim_interface:msg/ElectricsysCom.idl
// generated code does not contain a copyright notice
#include "autoaim_interface/msg/electricsys_com__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `sof`
#include "rosidl_generator_c/string_functions.h"

bool
autoaim_interface__msg__ElectricsysCom__init(autoaim_interface__msg__ElectricsysCom * msg)
{
  if (!msg) {
    return false;
  }
  // sof
  if (!rosidl_generator_c__String__init(&msg->sof)) {
    autoaim_interface__msg__ElectricsysCom__fini(msg);
    return false;
  }
  // target_abs_pitch
  // target_abs_yaw
  // reach_unix_time
  // fire_times
  // reserved_slot
  return true;
}

void
autoaim_interface__msg__ElectricsysCom__fini(autoaim_interface__msg__ElectricsysCom * msg)
{
  if (!msg) {
    return;
  }
  // sof
  rosidl_generator_c__String__fini(&msg->sof);
  // target_abs_pitch
  // target_abs_yaw
  // reach_unix_time
  // fire_times
  // reserved_slot
}

autoaim_interface__msg__ElectricsysCom *
autoaim_interface__msg__ElectricsysCom__create()
{
  autoaim_interface__msg__ElectricsysCom * msg = (autoaim_interface__msg__ElectricsysCom *)malloc(sizeof(autoaim_interface__msg__ElectricsysCom));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(autoaim_interface__msg__ElectricsysCom));
  bool success = autoaim_interface__msg__ElectricsysCom__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
autoaim_interface__msg__ElectricsysCom__destroy(autoaim_interface__msg__ElectricsysCom * msg)
{
  if (msg) {
    autoaim_interface__msg__ElectricsysCom__fini(msg);
  }
  free(msg);
}


bool
autoaim_interface__msg__ElectricsysCom__Sequence__init(autoaim_interface__msg__ElectricsysCom__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  autoaim_interface__msg__ElectricsysCom * data = NULL;
  if (size) {
    data = (autoaim_interface__msg__ElectricsysCom *)calloc(size, sizeof(autoaim_interface__msg__ElectricsysCom));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = autoaim_interface__msg__ElectricsysCom__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        autoaim_interface__msg__ElectricsysCom__fini(&data[i - 1]);
      }
      free(data);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
autoaim_interface__msg__ElectricsysCom__Sequence__fini(autoaim_interface__msg__ElectricsysCom__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      autoaim_interface__msg__ElectricsysCom__fini(&array->data[i]);
    }
    free(array->data);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

autoaim_interface__msg__ElectricsysCom__Sequence *
autoaim_interface__msg__ElectricsysCom__Sequence__create(size_t size)
{
  autoaim_interface__msg__ElectricsysCom__Sequence * array = (autoaim_interface__msg__ElectricsysCom__Sequence *)malloc(sizeof(autoaim_interface__msg__ElectricsysCom__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = autoaim_interface__msg__ElectricsysCom__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
autoaim_interface__msg__ElectricsysCom__Sequence__destroy(autoaim_interface__msg__ElectricsysCom__Sequence * array)
{
  if (array) {
    autoaim_interface__msg__ElectricsysCom__Sequence__fini(array);
  }
  free(array);
}
