// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from autoaim_interface:msg/DetectResult.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "autoaim_interface/msg/detect_result__rosidl_typesupport_introspection_c.h"
#include "autoaim_interface/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "autoaim_interface/msg/detect_result__struct.h"


// Include directives for member types
// Member `detect_result`
#include "autoaim_interface/msg/each_detect_result.h"
// Member `detect_result`
#include "autoaim_interface/msg/each_detect_result__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

size_t DetectResult__rosidl_typesupport_introspection_c__size_function__EachDetectResult__detect_result(
  const void * untyped_member)
{
  const autoaim_interface__msg__EachDetectResult__Sequence * member =
    (const autoaim_interface__msg__EachDetectResult__Sequence *)(untyped_member);
  return member->size;
}

const void * DetectResult__rosidl_typesupport_introspection_c__get_const_function__EachDetectResult__detect_result(
  const void * untyped_member, size_t index)
{
  const autoaim_interface__msg__EachDetectResult__Sequence * member =
    (const autoaim_interface__msg__EachDetectResult__Sequence *)(untyped_member);
  return &member->data[index];
}

void * DetectResult__rosidl_typesupport_introspection_c__get_function__EachDetectResult__detect_result(
  void * untyped_member, size_t index)
{
  autoaim_interface__msg__EachDetectResult__Sequence * member =
    (autoaim_interface__msg__EachDetectResult__Sequence *)(untyped_member);
  return &member->data[index];
}

bool DetectResult__rosidl_typesupport_introspection_c__resize_function__EachDetectResult__detect_result(
  void * untyped_member, size_t size)
{
  autoaim_interface__msg__EachDetectResult__Sequence * member =
    (autoaim_interface__msg__EachDetectResult__Sequence *)(untyped_member);
  autoaim_interface__msg__EachDetectResult__Sequence__fini(member);
  return autoaim_interface__msg__EachDetectResult__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember DetectResult__rosidl_typesupport_introspection_c__DetectResult_message_member_array[1] = {
  {
    "detect_result",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(autoaim_interface__msg__DetectResult, detect_result),  // bytes offset in struct
    NULL,  // default value
    DetectResult__rosidl_typesupport_introspection_c__size_function__EachDetectResult__detect_result,  // size() function pointer
    DetectResult__rosidl_typesupport_introspection_c__get_const_function__EachDetectResult__detect_result,  // get_const(index) function pointer
    DetectResult__rosidl_typesupport_introspection_c__get_function__EachDetectResult__detect_result,  // get(index) function pointer
    DetectResult__rosidl_typesupport_introspection_c__resize_function__EachDetectResult__detect_result  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers DetectResult__rosidl_typesupport_introspection_c__DetectResult_message_members = {
  "autoaim_interface__msg",  // message namespace
  "DetectResult",  // message name
  1,  // number of fields
  sizeof(autoaim_interface__msg__DetectResult),
  DetectResult__rosidl_typesupport_introspection_c__DetectResult_message_member_array  // message members
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t DetectResult__rosidl_typesupport_introspection_c__DetectResult_message_type_support_handle = {
  0,
  &DetectResult__rosidl_typesupport_introspection_c__DetectResult_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_autoaim_interface
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, autoaim_interface, msg, DetectResult)() {
  DetectResult__rosidl_typesupport_introspection_c__DetectResult_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, autoaim_interface, msg, EachDetectResult)();
  if (!DetectResult__rosidl_typesupport_introspection_c__DetectResult_message_type_support_handle.typesupport_identifier) {
    DetectResult__rosidl_typesupport_introspection_c__DetectResult_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &DetectResult__rosidl_typesupport_introspection_c__DetectResult_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
