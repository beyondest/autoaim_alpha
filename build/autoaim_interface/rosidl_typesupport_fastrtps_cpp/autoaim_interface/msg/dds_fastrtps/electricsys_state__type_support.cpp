// generated from rosidl_typesupport_fastrtps_cpp/resource/idl__type_support.cpp.em
// with input from autoaim_interface:msg/ElectricsysState.idl
// generated code does not contain a copyright notice
#include "autoaim_interface/msg/electricsys_state__rosidl_typesupport_fastrtps_cpp.hpp"
#include "autoaim_interface/msg/electricsys_state__struct.hpp"

#include <limits>
#include <stdexcept>
#include <string>
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_fastrtps_cpp/identifier.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_fastrtps_cpp/wstring_conversion.hpp"
#include "fastcdr/Cdr.h"


// forward declaration of message dependencies and their conversion functions

namespace autoaim_interface
{

namespace msg
{

namespace typesupport_fastrtps_cpp
{

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_autoaim_interface
cdr_serialize(
  const autoaim_interface::msg::ElectricsysState & ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Member: cur_yaw
  cdr << ros_message.cur_yaw;
  // Member: cur_pitch
  cdr << ros_message.cur_pitch;
  // Member: unix_time
  cdr << ros_message.unix_time;
  // Member: sentry_health
  cdr << ros_message.sentry_health;
  // Member: nullbyte
  cdr << ros_message.nullbyte;
  return true;
}

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_autoaim_interface
cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  autoaim_interface::msg::ElectricsysState & ros_message)
{
  // Member: cur_yaw
  cdr >> ros_message.cur_yaw;

  // Member: cur_pitch
  cdr >> ros_message.cur_pitch;

  // Member: unix_time
  cdr >> ros_message.unix_time;

  // Member: sentry_health
  cdr >> ros_message.sentry_health;

  // Member: nullbyte
  cdr >> ros_message.nullbyte;

  return true;
}

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_autoaim_interface
get_serialized_size(
  const autoaim_interface::msg::ElectricsysState & ros_message,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Member: cur_yaw
  {
    size_t item_size = sizeof(ros_message.cur_yaw);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: cur_pitch
  {
    size_t item_size = sizeof(ros_message.cur_pitch);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: unix_time
  {
    size_t item_size = sizeof(ros_message.unix_time);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: sentry_health
  {
    size_t item_size = sizeof(ros_message.sentry_health);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: nullbyte
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message.nullbyte.size() + 1);

  return current_alignment - initial_alignment;
}

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_autoaim_interface
max_serialized_size_ElectricsysState(
  bool & full_bounded,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;
  (void)full_bounded;


  // Member: cur_yaw
  {
    size_t array_size = 1;

    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Member: cur_pitch
  {
    size_t array_size = 1;

    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Member: unix_time
  {
    size_t array_size = 1;

    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Member: sentry_health
  {
    size_t array_size = 1;

    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Member: nullbyte
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

static bool _ElectricsysState__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  auto typed_message =
    static_cast<const autoaim_interface::msg::ElectricsysState *>(
    untyped_ros_message);
  return cdr_serialize(*typed_message, cdr);
}

static bool _ElectricsysState__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  auto typed_message =
    static_cast<autoaim_interface::msg::ElectricsysState *>(
    untyped_ros_message);
  return cdr_deserialize(cdr, *typed_message);
}

static uint32_t _ElectricsysState__get_serialized_size(
  const void * untyped_ros_message)
{
  auto typed_message =
    static_cast<const autoaim_interface::msg::ElectricsysState *>(
    untyped_ros_message);
  return static_cast<uint32_t>(get_serialized_size(*typed_message, 0));
}

static size_t _ElectricsysState__max_serialized_size(bool & full_bounded)
{
  return max_serialized_size_ElectricsysState(full_bounded, 0);
}

static message_type_support_callbacks_t _ElectricsysState__callbacks = {
  "autoaim_interface::msg",
  "ElectricsysState",
  _ElectricsysState__cdr_serialize,
  _ElectricsysState__cdr_deserialize,
  _ElectricsysState__get_serialized_size,
  _ElectricsysState__max_serialized_size
};

static rosidl_message_type_support_t _ElectricsysState__handle = {
  rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
  &_ElectricsysState__callbacks,
  get_message_typesupport_handle_function,
};

}  // namespace typesupport_fastrtps_cpp

}  // namespace msg

}  // namespace autoaim_interface

namespace rosidl_typesupport_fastrtps_cpp
{

template<>
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_EXPORT_autoaim_interface
const rosidl_message_type_support_t *
get_message_type_support_handle<autoaim_interface::msg::ElectricsysState>()
{
  return &autoaim_interface::msg::typesupport_fastrtps_cpp::_ElectricsysState__handle;
}

}  // namespace rosidl_typesupport_fastrtps_cpp

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, autoaim_interface, msg, ElectricsysState)() {
  return &autoaim_interface::msg::typesupport_fastrtps_cpp::_ElectricsysState__handle;
}

#ifdef __cplusplus
}
#endif
