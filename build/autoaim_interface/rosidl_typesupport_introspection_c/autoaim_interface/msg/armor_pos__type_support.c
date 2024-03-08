// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from autoaim_interface:msg/ArmorPos.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "autoaim_interface/msg/armor_pos__rosidl_typesupport_introspection_c.h"
#include "autoaim_interface/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "autoaim_interface/msg/armor_pos__struct.h"


// Include directives for member types
// Member `armor_name`
#include "rosidl_generator_c/string_functions.h"
// Member `pose`
#include "geometry_msgs/msg/pose_stamped.h"
// Member `pose`
#include "geometry_msgs/msg/pose_stamped__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

static rosidl_typesupport_introspection_c__MessageMember ArmorPos__rosidl_typesupport_introspection_c__ArmorPos_message_member_array[4] = {
  {
    "armor_name",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(autoaim_interface__msg__ArmorPos, armor_name),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "armor_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(autoaim_interface__msg__ArmorPos, armor_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "confidence",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(autoaim_interface__msg__ArmorPos, confidence),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "pose",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(autoaim_interface__msg__ArmorPos, pose),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers ArmorPos__rosidl_typesupport_introspection_c__ArmorPos_message_members = {
  "autoaim_interface__msg",  // message namespace
  "ArmorPos",  // message name
  4,  // number of fields
  sizeof(autoaim_interface__msg__ArmorPos),
  ArmorPos__rosidl_typesupport_introspection_c__ArmorPos_message_member_array  // message members
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t ArmorPos__rosidl_typesupport_introspection_c__ArmorPos_message_type_support_handle = {
  0,
  &ArmorPos__rosidl_typesupport_introspection_c__ArmorPos_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_autoaim_interface
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, autoaim_interface, msg, ArmorPos)() {
  ArmorPos__rosidl_typesupport_introspection_c__ArmorPos_message_member_array[3].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, PoseStamped)();
  if (!ArmorPos__rosidl_typesupport_introspection_c__ArmorPos_message_type_support_handle.typesupport_identifier) {
    ArmorPos__rosidl_typesupport_introspection_c__ArmorPos_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &ArmorPos__rosidl_typesupport_introspection_c__ArmorPos_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
