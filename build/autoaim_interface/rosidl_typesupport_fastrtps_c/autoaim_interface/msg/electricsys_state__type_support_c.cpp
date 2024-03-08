// generated from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
// with input from autoaim_interface:msg/ElectricsysState.idl
// generated code does not contain a copyright notice
#include "autoaim_interface/msg/electricsys_state__rosidl_typesupport_fastrtps_c.h"


#include <cassert>
#include <limits>
#include <string>
#include "rosidl_typesupport_fastrtps_c/identifier.h"
#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "autoaim_interface/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "autoaim_interface/msg/electricsys_state__struct.h"
#include "autoaim_interface/msg/electricsys_state__functions.h"
#include "fastcdr/Cdr.h"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// includes and forward declarations of message dependencies and their conversion functions

#if defined(__cplusplus)
extern "C"
{
#endif

#include "rosidl_generator_c/string.h"  // nullbyte
#include "rosidl_generator_c/string_functions.h"  // nullbyte

// forward declare type support functions


using _ElectricsysState__ros_msg_type = autoaim_interface__msg__ElectricsysState;

static bool _ElectricsysState__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const _ElectricsysState__ros_msg_type * ros_message = static_cast<const _ElectricsysState__ros_msg_type *>(untyped_ros_message);
  // Field name: cur_yaw
  {
    cdr << ros_message->cur_yaw;
  }

  // Field name: cur_pitch
  {
    cdr << ros_message->cur_pitch;
  }

  // Field name: unix_time
  {
    cdr << ros_message->unix_time;
  }

  // Field name: sentry_health
  {
    cdr << ros_message->sentry_health;
  }

  // Field name: nullbyte
  {
    const rosidl_generator_c__String * str = &ros_message->nullbyte;
    if (str->capacity == 0 || str->capacity <= str->size) {
      fprintf(stderr, "string capacity not greater than size\n");
      return false;
    }
    if (str->data[str->size] != '\0') {
      fprintf(stderr, "string not null-terminated\n");
      return false;
    }
    cdr << str->data;
  }

  return true;
}

static bool _ElectricsysState__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  _ElectricsysState__ros_msg_type * ros_message = static_cast<_ElectricsysState__ros_msg_type *>(untyped_ros_message);
  // Field name: cur_yaw
  {
    cdr >> ros_message->cur_yaw;
  }

  // Field name: cur_pitch
  {
    cdr >> ros_message->cur_pitch;
  }

  // Field name: unix_time
  {
    cdr >> ros_message->unix_time;
  }

  // Field name: sentry_health
  {
    cdr >> ros_message->sentry_health;
  }

  // Field name: nullbyte
  {
    std::string tmp;
    cdr >> tmp;
    if (!ros_message->nullbyte.data) {
      rosidl_generator_c__String__init(&ros_message->nullbyte);
    }
    bool succeeded = rosidl_generator_c__String__assign(
      &ros_message->nullbyte,
      tmp.c_str());
    if (!succeeded) {
      fprintf(stderr, "failed to assign string into field 'nullbyte'\n");
      return false;
    }
  }

  return true;
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_autoaim_interface
size_t get_serialized_size_autoaim_interface__msg__ElectricsysState(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _ElectricsysState__ros_msg_type * ros_message = static_cast<const _ElectricsysState__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // field.name cur_yaw
  {
    size_t item_size = sizeof(ros_message->cur_yaw);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name cur_pitch
  {
    size_t item_size = sizeof(ros_message->cur_pitch);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name unix_time
  {
    size_t item_size = sizeof(ros_message->unix_time);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name sentry_health
  {
    size_t item_size = sizeof(ros_message->sentry_health);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name nullbyte
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message->nullbyte.size + 1);

  return current_alignment - initial_alignment;
}

static uint32_t _ElectricsysState__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_autoaim_interface__msg__ElectricsysState(
      untyped_ros_message, 0));
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_autoaim_interface
size_t max_serialized_size_autoaim_interface__msg__ElectricsysState(
  bool & full_bounded,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;
  (void)full_bounded;

  // member: cur_yaw
  {
    size_t array_size = 1;

    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }
  // member: cur_pitch
  {
    size_t array_size = 1;

    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }
  // member: unix_time
  {
    size_t array_size = 1;

    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }
  // member: sentry_health
  {
    size_t array_size = 1;

    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }
  // member: nullbyte
  {
    size_t array_size = 1;

    full_bounded = false;
    for (size_t index = 0; index < array_size; ++index) {
      current_alignment += padding +
        eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
        1;
    }
  }

  return current_alignment - initial_alignment;
}

static size_t _ElectricsysState__max_serialized_size(bool & full_bounded)
{
  return max_serialized_size_autoaim_interface__msg__ElectricsysState(
    full_bounded, 0);
}


static message_type_support_callbacks_t __callbacks_ElectricsysState = {
  "autoaim_interface::msg",
  "ElectricsysState",
  _ElectricsysState__cdr_serialize,
  _ElectricsysState__cdr_deserialize,
  _ElectricsysState__get_serialized_size,
  _ElectricsysState__max_serialized_size
};

static rosidl_message_type_support_t _ElectricsysState__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_ElectricsysState,
  get_message_typesupport_handle_function,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, autoaim_interface, msg, ElectricsysState)() {
  return &_ElectricsysState__type_support;
}

#if defined(__cplusplus)
}
#endif
