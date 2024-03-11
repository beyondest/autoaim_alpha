// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from autoaim_interface:msg/ElectricsysState.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "autoaim_interface/msg/electricsys_state__rosidl_typesupport_introspection_c.h"
#include "autoaim_interface/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "autoaim_interface/msg/electricsys_state__struct.h"


// Include directives for member types
// Member `nullbyte`
#include "rosidl_generator_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

static rosidl_typesupport_introspection_c__MessageMember ElectricsysState__rosidl_typesupport_introspection_c__ElectricsysState_message_member_array[5] = {
  {
    "cur_yaw",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(autoaim_interface__msg__ElectricsysState, cur_yaw),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "cur_pitch",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(autoaim_interface__msg__ElectricsysState, cur_pitch),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "unix_time",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(autoaim_interface__msg__ElectricsysState, unix_time),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "sentry_health",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(autoaim_interface__msg__ElectricsysState, sentry_health),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "nullbyte",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(autoaim_interface__msg__ElectricsysState, nullbyte),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers ElectricsysState__rosidl_typesupport_introspection_c__ElectricsysState_message_members = {
  "autoaim_interface__msg",  // message namespace
  "ElectricsysState",  // message name
  5,  // number of fields
  sizeof(autoaim_interface__msg__ElectricsysState),
  ElectricsysState__rosidl_typesupport_introspection_c__ElectricsysState_message_member_array  // message members
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t ElectricsysState__rosidl_typesupport_introspection_c__ElectricsysState_message_type_support_handle = {
  0,
  &ElectricsysState__rosidl_typesupport_introspection_c__ElectricsysState_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_autoaim_interface
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, autoaim_interface, msg, ElectricsysState)() {
  if (!ElectricsysState__rosidl_typesupport_introspection_c__ElectricsysState_message_type_support_handle.typesupport_identifier) {
    ElectricsysState__rosidl_typesupport_introspection_c__ElectricsysState_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &ElectricsysState__rosidl_typesupport_introspection_c__ElectricsysState_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
