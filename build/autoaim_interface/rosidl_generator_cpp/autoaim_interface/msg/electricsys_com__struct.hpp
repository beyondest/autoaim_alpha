// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from autoaim_interface:msg/ElectricsysCom.idl
// generated code does not contain a copyright notice

#ifndef AUTOAIM_INTERFACE__MSG__ELECTRICSYS_COM__STRUCT_HPP_
#define AUTOAIM_INTERFACE__MSG__ELECTRICSYS_COM__STRUCT_HPP_

#include <rosidl_generator_cpp/bounded_vector.hpp>
#include <rosidl_generator_cpp/message_initialization.hpp>
#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

// Protect against ERROR being predefined on Windows, in case somebody defines a
// constant by that name.
#if defined(_WIN32)
  #if defined(ERROR)
    #undef ERROR
  #endif
  #if defined(NO_ERROR)
    #undef NO_ERROR
  #endif
#endif

#ifndef _WIN32
# define DEPRECATED__autoaim_interface__msg__ElectricsysCom __attribute__((deprecated))
#else
# define DEPRECATED__autoaim_interface__msg__ElectricsysCom __declspec(deprecated)
#endif

namespace autoaim_interface
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct ElectricsysCom_
{
  using Type = ElectricsysCom_<ContainerAllocator>;

  explicit ElectricsysCom_(rosidl_generator_cpp::MessageInitialization _init = rosidl_generator_cpp::MessageInitialization::ALL)
  {
    if (rosidl_generator_cpp::MessageInitialization::ALL == _init ||
      rosidl_generator_cpp::MessageInitialization::ZERO == _init)
    {
      this->sof = "";
      this->target_abs_pitch = 0.0;
      this->target_abs_yaw = 0.0;
      this->reach_unix_time = 0.0;
      this->fire_times = 0ll;
      this->reserved_slot = 0ll;
    }
  }

  explicit ElectricsysCom_(const ContainerAllocator & _alloc, rosidl_generator_cpp::MessageInitialization _init = rosidl_generator_cpp::MessageInitialization::ALL)
  : sof(_alloc)
  {
    if (rosidl_generator_cpp::MessageInitialization::ALL == _init ||
      rosidl_generator_cpp::MessageInitialization::ZERO == _init)
    {
      this->sof = "";
      this->target_abs_pitch = 0.0;
      this->target_abs_yaw = 0.0;
      this->reach_unix_time = 0.0;
      this->fire_times = 0ll;
      this->reserved_slot = 0ll;
    }
  }

  // field types and members
  using _sof_type =
    std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other>;
  _sof_type sof;
  using _target_abs_pitch_type =
    double;
  _target_abs_pitch_type target_abs_pitch;
  using _target_abs_yaw_type =
    double;
  _target_abs_yaw_type target_abs_yaw;
  using _reach_unix_time_type =
    double;
  _reach_unix_time_type reach_unix_time;
  using _fire_times_type =
    int64_t;
  _fire_times_type fire_times;
  using _reserved_slot_type =
    int64_t;
  _reserved_slot_type reserved_slot;

  // setters for named parameter idiom
  Type & set__sof(
    const std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other> & _arg)
  {
    this->sof = _arg;
    return *this;
  }
  Type & set__target_abs_pitch(
    const double & _arg)
  {
    this->target_abs_pitch = _arg;
    return *this;
  }
  Type & set__target_abs_yaw(
    const double & _arg)
  {
    this->target_abs_yaw = _arg;
    return *this;
  }
  Type & set__reach_unix_time(
    const double & _arg)
  {
    this->reach_unix_time = _arg;
    return *this;
  }
  Type & set__fire_times(
    const int64_t & _arg)
  {
    this->fire_times = _arg;
    return *this;
  }
  Type & set__reserved_slot(
    const int64_t & _arg)
  {
    this->reserved_slot = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    autoaim_interface::msg::ElectricsysCom_<ContainerAllocator> *;
  using ConstRawPtr =
    const autoaim_interface::msg::ElectricsysCom_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<autoaim_interface::msg::ElectricsysCom_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<autoaim_interface::msg::ElectricsysCom_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      autoaim_interface::msg::ElectricsysCom_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<autoaim_interface::msg::ElectricsysCom_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      autoaim_interface::msg::ElectricsysCom_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<autoaim_interface::msg::ElectricsysCom_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<autoaim_interface::msg::ElectricsysCom_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<autoaim_interface::msg::ElectricsysCom_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__autoaim_interface__msg__ElectricsysCom
    std::shared_ptr<autoaim_interface::msg::ElectricsysCom_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__autoaim_interface__msg__ElectricsysCom
    std::shared_ptr<autoaim_interface::msg::ElectricsysCom_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const ElectricsysCom_ & other) const
  {
    if (this->sof != other.sof) {
      return false;
    }
    if (this->target_abs_pitch != other.target_abs_pitch) {
      return false;
    }
    if (this->target_abs_yaw != other.target_abs_yaw) {
      return false;
    }
    if (this->reach_unix_time != other.reach_unix_time) {
      return false;
    }
    if (this->fire_times != other.fire_times) {
      return false;
    }
    if (this->reserved_slot != other.reserved_slot) {
      return false;
    }
    return true;
  }
  bool operator!=(const ElectricsysCom_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct ElectricsysCom_

// alias to use template instance with default allocator
using ElectricsysCom =
  autoaim_interface::msg::ElectricsysCom_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace autoaim_interface

#endif  // AUTOAIM_INTERFACE__MSG__ELECTRICSYS_COM__STRUCT_HPP_
