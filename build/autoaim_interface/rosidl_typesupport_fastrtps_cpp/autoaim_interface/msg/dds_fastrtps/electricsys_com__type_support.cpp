// generated from rosidl_typesupport_fastrtps_cpp/resource/idl__type_support.cpp.em
// with input from autoaim_interface:msg/ElectricsysCom.idl
// generated code does not contain a copyright notice
#include "autoaim_interface/msg/electricsys_com__rosidl_typesupport_fastrtps_cpp.hpp"
#include "autoaim_interface/msg/electricsys_com__struct.hpp"

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
  const autoaim_interface::msg::ElectricsysCom & ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Member: sof
  cdr << ros_message.sof;
  // Member: target_abs_pitch
  cdr << ros_message.target_abs_pitch;
  // Member: target_abs_yaw
  cdr << ros_message.target_abs_yaw;
  // Member: reach_unix_time
  cdr << ros_message.reach_unix_time;
  // Member: fire_times
  cdr << ros_message.fire_times;
  // Member: reserved_slot
  cdr << ros_message.reserved_slot;
  return true;
}

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_autoaim_interface
cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  autoaim_interface::msg::ElectricsysCom & ros_message)
{
  // Member: sof
  cdr >> ros_message.sof;

  // Member: target_abs_pitch
  cdr >> ros_message.target_abs_pitch;

  // Member: target_abs_yaw
  cdr >> ros_message.target_abs_yaw;

  // Member: reach_unix_time
  cdr >> ros_message.reach_unix_time;

  // Member: fire_times
  cdr >> ros_message.fire_times;

  // Member: reserved_slot
  cdr >> ros_message.reserved_slot;

  return true;
}

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_autoaim_interface
get_serialized_size(
  const autoaim_interface::msg::ElectricsysCom & ros_message,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Member: sof
  current_alignment += padding +
    eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
    (ros_message.sof.size() + 1);
  // Member: target_abs_pitch
  {
    size_t item_size = sizeof(ros_message.target_abs_pitch);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: target_abs_yaw
  {
    size_t item_size = sizeof(ros_message.target_abs_yaw);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: reach_unix_time
  {
    size_t item_size = sizeof(ros_message.reach_unix_time);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: fire_times
  {
    size_t item_size = sizeof(ros_message.fire_times);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: reserved_slot
  {
    size_t item_size = sizeof(ros_message.reserved_slot);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_autoaim_interface
max_serialized_size_ElectricsysCom(
  bool & full_bounded,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;
  (void)full_bounded;


  // Member: sof
  {
    size_t array_size = 1;

    full_bounded = false;
    for (size_t index = 0; index < array_size; ++index) {
      current_alignment += padding +
        eprosima::fastcdr::Cdr::alignment(current_alignment, padding) +
        1;
    }
  }

  // Member: target_abs_pitch
  {
    size_t array_size = 1;

    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Member: target_abs_yaw
  {
    size_t array_size = 1;

    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Member: reach_unix_time
  {
    size_t array_size = 1;

    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Member: fire_times
  {
    size_t array_size = 1;

    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  // Member: reserved_slot
  {
    size_t array_size = 1;

    current_alignment += array_size * sizeof(uint64_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint64_t));
  }

  return current_alignment - initial_alignment;
}

static bool _ElectricsysCom__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  auto typed_message =
    static_cast<const autoaim_interface::msg::ElectricsysCom *>(
    untyped_ros_message);
  return cdr_serialize(*typed_message, cdr);
}

static bool _ElectricsysCom__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  auto typed_message =
    static_cast<autoaim_interface::msg::ElectricsysCom *>(
    untyped_ros_message);
  return cdr_deserialize(cdr, *typed_message);
}

static uint32_t _ElectricsysCom__get_serialized_size(
  const void * untyped_ros_message)
{
  auto typed_message =
    static_cast<const autoaim_interface::msg::ElectricsysCom *>(
    untyped_ros_message);
  return static_cast<uint32_t>(get_serialized_size(*typed_message, 0));
}

static size_t _ElectricsysCom__max_serialized_size(bool & full_bounded)
{
  return max_serialized_size_ElectricsysCom(full_bounded, 0);
}

static message_type_support_callbacks_t _ElectricsysCom__callbacks = {
  "autoaim_interface::msg",
  "ElectricsysCom",
  _ElectricsysCom__cdr_serialize,
  _ElectricsysCom__cdr_deserialize,
  _ElectricsysCom__get_serialized_size,
  _ElectricsysCom__max_serialized_size
};

static rosidl_message_type_support_t _ElectricsysCom__handle = {
  rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
  &_ElectricsysCom__callbacks,
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
get_message_type_support_handle<autoaim_interface::msg::ElectricsysCom>()
{
  return &autoaim_interface::msg::typesupport_fastrtps_cpp::_ElectricsysCom__handle;
}

}  // namespace rosidl_typesupport_fastrtps_cpp

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, autoaim_interface, msg, ElectricsysCom)() {
  return &autoaim_interface::msg::typesupport_fastrtps_cpp::_ElectricsysCom__handle;
}

#ifdef __cplusplus
}
#endif
