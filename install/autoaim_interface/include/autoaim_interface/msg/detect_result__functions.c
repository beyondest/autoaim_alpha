// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from autoaim_interface:msg/DetectResult.idl
// generated code does not contain a copyright notice
#include "autoaim_interface/msg/detect_result__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// Include directives for member types
// Member `detect_result`
#include "autoaim_interface/msg/each_detect_result__functions.h"

bool
autoaim_interface__msg__DetectResult__init(autoaim_interface__msg__DetectResult * msg)
{
  if (!msg) {
    return false;
  }
  // detect_result
  if (!autoaim_interface__msg__EachDetectResult__Sequence__init(&msg->detect_result, 0)) {
    autoaim_interface__msg__DetectResult__fini(msg);
    return false;
  }
  return true;
}

void
autoaim_interface__msg__DetectResult__fini(autoaim_interface__msg__DetectResult * msg)
{
  if (!msg) {
    return;
  }
  // detect_result
  autoaim_interface__msg__EachDetectResult__Sequence__fini(&msg->detect_result);
}

autoaim_interface__msg__DetectResult *
autoaim_interface__msg__DetectResult__create()
{
  autoaim_interface__msg__DetectResult * msg = (autoaim_interface__msg__DetectResult *)malloc(sizeof(autoaim_interface__msg__DetectResult));
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(autoaim_interface__msg__DetectResult));
  bool success = autoaim_interface__msg__DetectResult__init(msg);
  if (!success) {
    free(msg);
    return NULL;
  }
  return msg;
}

void
autoaim_interface__msg__DetectResult__destroy(autoaim_interface__msg__DetectResult * msg)
{
  if (msg) {
    autoaim_interface__msg__DetectResult__fini(msg);
  }
  free(msg);
}


bool
autoaim_interface__msg__DetectResult__Sequence__init(autoaim_interface__msg__DetectResult__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  autoaim_interface__msg__DetectResult * data = NULL;
  if (size) {
    data = (autoaim_interface__msg__DetectResult *)calloc(size, sizeof(autoaim_interface__msg__DetectResult));
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = autoaim_interface__msg__DetectResult__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        autoaim_interface__msg__DetectResult__fini(&data[i - 1]);
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
autoaim_interface__msg__DetectResult__Sequence__fini(autoaim_interface__msg__DetectResult__Sequence * array)
{
  if (!array) {
    return;
  }
  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      autoaim_interface__msg__DetectResult__fini(&array->data[i]);
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

autoaim_interface__msg__DetectResult__Sequence *
autoaim_interface__msg__DetectResult__Sequence__create(size_t size)
{
  autoaim_interface__msg__DetectResult__Sequence * array = (autoaim_interface__msg__DetectResult__Sequence *)malloc(sizeof(autoaim_interface__msg__DetectResult__Sequence));
  if (!array) {
    return NULL;
  }
  bool success = autoaim_interface__msg__DetectResult__Sequence__init(array, size);
  if (!success) {
    free(array);
    return NULL;
  }
  return array;
}

void
autoaim_interface__msg__DetectResult__Sequence__destroy(autoaim_interface__msg__DetectResult__Sequence * array)
{
  if (array) {
    autoaim_interface__msg__DetectResult__Sequence__fini(array);
  }
  free(array);
}
