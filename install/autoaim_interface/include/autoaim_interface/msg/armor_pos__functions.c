// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from autoaim_interface:msg/ArmorPos.idl
// generated code does not contain a copyright notice
#include "autoaim_interface/msg/armor_pos__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `armor_name`
#include "rosidl_generator_c/string_functions.h"
// Member `pose`
#include "geometry_msgs/msg/pose_stamped__functions.h"

bool
autoaim_interface__msg__ArmorPos__init(autoaim_interface__msg__ArmorPos * msg)
{
  if (!msg) {
    return false;
  }
  // armor_name
  if (!rosidl_generator_c__String__init(&msg->armor_name)) {
    autoaim_interface__msg__ArmorPos__fini(msg);
    return false;
  }
  // armor_id
  // confidence
  // pose
  if (!geometry_msgs__msg__PoseStamped__init(&msg->pose)) {
    autoaim_interface__msg__ArmorPos__fini(msg);
    return false;
  }
  return true;
}

void
autoaim_interface__msg__ArmorPos__fini(autoaim_interface__msg__ArmorPos * msg)
{
  if (!msg) {
    return;
  }
  // armor_name
  rosidl_generator_c__String__fini(&msg->armor_name);
  // armor_id
  // confidence
  // pose
  geometry_msgs__msg__PoseStamped__fini(&msg->pose);
}

autoaim_interface__msg__ArmorPos *
autoaim_interface__msg__ArmorPos__create()
{
  autoaim_interface__msg__ArmorPos * msg = (autoaim_interface__msg__ArmorPos *)malloc(sizeof(autoaim_interface__msg__ArmorPos));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(autoaim_interface__msg__ArmorPos));
  bool success = autoaim_interface__msg__ArmorPos__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
autoaim_interface__msg__ArmorPos__destroy(autoaim_interface__msg__ArmorPos * msg)
{
  if (msg) {
    autoaim_interface__msg__ArmorPos__fini(msg);
  }
  free(msg);
}


bool
autoaim_interface__msg__ArmorPos__Sequence__init(autoaim_interface__msg__ArmorPos__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  autoaim_interface__msg__ArmorPos * data = NULL;
  if (size) {
    data = (autoaim_interface__msg__ArmorPos *)calloc(size, sizeof(autoaim_interface__msg__ArmorPos));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = autoaim_interface__msg__ArmorPos__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        autoaim_interface__msg__ArmorPos__fini(&data[i - 1]);
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
autoaim_interface__msg__ArmorPos__Sequence__fini(autoaim_interface__msg__ArmorPos__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      autoaim_interface__msg__ArmorPos__fini(&array->data[i]);
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

autoaim_interface__msg__ArmorPos__Sequence *
autoaim_interface__msg__ArmorPos__Sequence__create(size_t size)
{
  autoaim_interface__msg__ArmorPos__Sequence * array = (autoaim_interface__msg__ArmorPos__Sequence *)malloc(sizeof(autoaim_interface__msg__ArmorPos__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = autoaim_interface__msg__ArmorPos__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
autoaim_interface__msg__ArmorPos__Sequence__destroy(autoaim_interface__msg__ArmorPos__Sequence * array)
{
  if (array) {
    autoaim_interface__msg__ArmorPos__Sequence__fini(array);
  }
  free(array);
}
