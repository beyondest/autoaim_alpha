// generated from rosidl_typesupport_fastrtps_cpp/resource/idl__type_support.cpp.em
// with input from autoaim_interface:msg/DetectResult.idl
// generated code does not contain a copyright notice
#include "autoaim_interface/msg/detect_result__rosidl_typesupport_fastrtps_cpp.hpp"
#include "autoaim_interface/msg/detect_result__struct.hpp"

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
bool cdr_serialize(
  const autoaim_interface::msg::EachDetectResult &,
  eprosima::fastcdr::Cdr &);
bool cdr_deserialize(
  eprosima::fastcdr::Cdr &,
  autoaim_interface::msg::EachDetectResult &);
size_t get_serialized_size(
  const autoaim_interface::msg::EachDetectResult &,
  size_t current_alignment);
size_t
max_serialized_size_EachDetectResult(
  bool & full_bounded,
  size_t current_alignment);
}  // namespace typesupport_fastrtps_cpp
}  // namespace msg
}  // namespace autoaim_interface


namespace autoaim_interface
{

namespace msg
{

namespace typesupport_fastrtps_cpp
{

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_autoaim_interface
cdr_serialize(
  const autoaim_interface::msg::DetectResult & ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Member: detect_result
  {
    size_t size = ros_message.detect_result.size();
    cdr << static_cast<uint32_t>(size);
    for (size_t i = 0; i < size; i++) {
      autoaim_interface::msg::typesupport_fastrtps_cpp::cdr_serialize(
        ros_message.detect_result[i],
        cdr);
    }
  }
  return true;
}

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_autoaim_interface
cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  autoaim_interface::msg::DetectResult & ros_message)
{
  // Member: detect_result
  {
    uint32_t cdrSize;
    cdr >> cdrSize;
    size_t size = static_cast<size_t>(cdrSize);
    ros_message.detect_result.resize(size);
    for (size_t i = 0; i < size; i++) {
      autoaim_interface::msg::typesupport_fastrtps_cpp::cdr_deserialize(
        cdr, ros_message.detect_result[i]);
    }
  }

  return true;
}

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_autoaim_interface
get_serialized_size(
  const autoaim_interface::msg::DetectResult & ros_message,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Member: detect_result
  {
    size_t array_size = ros_message.detect_result.size();

    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);

    for (size_t index = 0; index < array_size; ++index) {
      current_alignment +=
        autoaim_interface::msg::typesupport_fastrtps_cpp::get_serialized_size(
        ros_message.detect_result[index], current_alignment);
    }
  }

  return current_alignment - initial_alignment;
}

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_autoaim_interface
max_serialized_size_DetectResult(
  bool & full_bounded,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;
  (void)full_bounded;


  // Member: detect_result
  {
    size_t array_size = 0;
    full_bounded = false;
    current_alignment += padding +
      eprosima::fastcdr::Cdr::alignment(current_alignment, padding);


    for (size_t index = 0; index < array_size; ++index) {
      current_alignment +=
        autoaim_interface::msg::typesupport_fastrtps_cpp::max_serialized_size_EachDetectResult(
        full_bounded, current_alignment);
    }
  }

  return current_alignment - initial_alignment;
}

static bool _DetectResult__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  auto typed_message =
    static_cast<const autoaim_interface::msg::DetectResult *>(
    untyped_ros_message);
  return cdr_serialize(*typed_message, cdr);
}

static bool _DetectResult__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  auto typed_message =
    static_cast<autoaim_interface::msg::DetectResult *>(
    untyped_ros_message);
  return cdr_deserialize(cdr, *typed_message);
}

static uint32_t _DetectResult__get_serialized_size(
  const void * untyped_ros_message)
{
  auto typed_message =
    static_cast<const autoaim_interface::msg::DetectResult *>(
    untyped_ros_message);
  return static_cast<uint32_t>(get_serialized_size(*typed_message, 0));
}

static size_t _DetectResult__max_serialized_size(bool & full_bounded)
{
  return max_serialized_size_DetectResult(full_bounded, 0);
}

static message_type_support_callbacks_t _DetectResult__callbacks = {
  "autoaim_interface::msg",
  "DetectResult",
  _DetectResult__cdr_serialize,
  _DetectResult__cdr_deserialize,
  _DetectResult__get_serialized_size,
  _DetectResult__max_serialized_size
};

static rosidl_message_type_support_t _DetectResult__handle = {
  rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
  &_DetectResult__callbacks,
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
get_message_type_support_handle<autoaim_interface::msg::DetectResult>()
{
  return &autoaim_interface::msg::typesupport_fastrtps_cpp::_DetectResult__handle;
}

}  // namespace rosidl_typesupport_fastrtps_cpp

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, autoaim_interface, msg, DetectResult)() {
  return &autoaim_interface::msg::typesupport_fastrtps_cpp::_DetectResult__handle;
}

#ifdef __cplusplus
}
#endif
