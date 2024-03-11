// generated from rosidl_generator_py/resource/_idl_support.c.em
// with input from autoaim_interface:msg/ElectricsysCom.idl
// generated code does not contain a copyright notice
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <Python.h>
#include <stdbool.h>
#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-function"
#endif
#include "numpy/ndarrayobject.h"
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif
#include "rosidl_generator_c/visibility_control.h"
#include "autoaim_interface/msg/electricsys_com__struct.h"
#include "autoaim_interface/msg/electricsys_com__functions.h"

#include "rosidl_generator_c/string.h"
#include "rosidl_generator_c/string_functions.h"


ROSIDL_GENERATOR_C_EXPORT
bool autoaim_interface__msg__electricsys_com__convert_from_py(PyObject * _pymsg, void * _ros_message)
{
  // check that the passed message is of the expected Python class
  {
    char full_classname_dest[54];
    {
      char * class_name = NULL;
      char * module_name = NULL;
      {
        PyObject * class_attr = PyObject_GetAttrString(_pymsg, "__class__");
        if (class_attr) {
          PyObject * name_attr = PyObject_GetAttrString(class_attr, "__name__");
          if (name_attr) {
            class_name = (char *)PyUnicode_1BYTE_DATA(name_attr);
            Py_DECREF(name_attr);
          }
          PyObject * module_attr = PyObject_GetAttrString(class_attr, "__module__");
          if (module_attr) {
            module_name = (char *)PyUnicode_1BYTE_DATA(module_attr);
            Py_DECREF(module_attr);
          }
          Py_DECREF(class_attr);
        }
      }
      if (!class_name || !module_name) {
        return false;
      }
      snprintf(full_classname_dest, sizeof(full_classname_dest), "%s.%s", module_name, class_name);
    }
    assert(strncmp(
        "autoaim_interface.msg._electricsys_com.ElectricsysCom",
        full_classname_dest, 53) == 0);
  }
  autoaim_interface__msg__ElectricsysCom * ros_message = _ros_message;
  {  // sof
    PyObject * field = PyObject_GetAttrString(_pymsg, "sof");
    if (!field) {
      return false;
    }
    assert(PyUnicode_Check(field));
    PyObject * encoded_field = PyUnicode_AsUTF8String(field);
    if (!encoded_field) {
      Py_DECREF(field);
      return false;
    }
    rosidl_generator_c__String__assign(&ros_message->sof, PyBytes_AS_STRING(encoded_field));
    Py_DECREF(encoded_field);
    Py_DECREF(field);
  }
  {  // target_abs_pitch
    PyObject * field = PyObject_GetAttrString(_pymsg, "target_abs_pitch");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->target_abs_pitch = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // target_abs_yaw
    PyObject * field = PyObject_GetAttrString(_pymsg, "target_abs_yaw");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->target_abs_yaw = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // reach_unix_time
    PyObject * field = PyObject_GetAttrString(_pymsg, "reach_unix_time");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->reach_unix_time = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // fire_times
    PyObject * field = PyObject_GetAttrString(_pymsg, "fire_times");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->fire_times = PyLong_AsLongLong(field);
    Py_DECREF(field);
  }
  {  // reserved_slot
    PyObject * field = PyObject_GetAttrString(_pymsg, "reserved_slot");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->reserved_slot = PyLong_AsLongLong(field);
    Py_DECREF(field);
  }

  return true;
}

ROSIDL_GENERATOR_C_EXPORT
PyObject * autoaim_interface__msg__electricsys_com__convert_to_py(void * raw_ros_message)
{
  /* NOTE(esteve): Call constructor of ElectricsysCom */
  PyObject * _pymessage = NULL;
  {
    PyObject * pymessage_module = PyImport_ImportModule("autoaim_interface.msg._electricsys_com");
    assert(pymessage_module);
    PyObject * pymessage_class = PyObject_GetAttrString(pymessage_module, "ElectricsysCom");
    assert(pymessage_class);
    Py_DECREF(pymessage_module);
    _pymessage = PyObject_CallObject(pymessage_class, NULL);
    Py_DECREF(pymessage_class);
    if (!_pymessage) {
      return NULL;
    }
  }
  autoaim_interface__msg__ElectricsysCom * ros_message = (autoaim_interface__msg__ElectricsysCom *)raw_ros_message;
  {  // sof
    PyObject * field = NULL;
    field = PyUnicode_DecodeUTF8(
      ros_message->sof.data,
      strlen(ros_message->sof.data),
      "strict");
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "sof", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // target_abs_pitch
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->target_abs_pitch);
    {
      int rc = PyObject_SetAttrString(_pymessage, "target_abs_pitch", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // target_abs_yaw
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->target_abs_yaw);
    {
      int rc = PyObject_SetAttrString(_pymessage, "target_abs_yaw", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // reach_unix_time
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->reach_unix_time);
    {
      int rc = PyObject_SetAttrString(_pymessage, "reach_unix_time", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // fire_times
    PyObject * field = NULL;
    field = PyLong_FromLongLong(ros_message->fire_times);
    {
      int rc = PyObject_SetAttrString(_pymessage, "fire_times", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // reserved_slot
    PyObject * field = NULL;
    field = PyLong_FromLongLong(ros_message->reserved_slot);
    {
      int rc = PyObject_SetAttrString(_pymessage, "reserved_slot", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }

  // ownership of _pymessage is transferred to the caller
  return _pymessage;
}
