// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from autoaim_interface:msg/ElectricsysState.idl
// generated code does not contain a copyright notice

#ifndef AUTOAIM_INTERFACE__MSG__ELECTRICSYS_STATE__STRUCT_HPP_
#define AUTOAIM_INTERFACE__MSG__ELECTRICSYS_STATE__STRUCT_HPP_

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
# define DEPRECATED__autoaim_interface__msg__ElectricsysState __attribute__((deprecated))
#else
# define DEPRECATED__autoaim_interface__msg__ElectricsysState __declspec(deprecated)
#endif

namespace autoaim_interface
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct ElectricsysState_
{
  using Type = ElectricsysState_<ContainerAllocator>;

  explicit ElectricsysState_(rosidl_generator_cpp::MessageInitialization _init = rosidl_generator_cpp::MessageInitialization::ALL)
  {
    if (rosidl_generator_cpp::MessageInitialization::ALL == _init ||
      rosidl_generator_cpp::MessageInitialization::ZERO == _init)
    {
      this->cur_yaw = 0.0;
      this->cur_pitch = 0.0;
      this->unix_time = 0.0;
      this->sentry_health = 0ll;
      this->nullbyte = "";
    }
  }

  explicit ElectricsysState_(const ContainerAllocator & _alloc, rosidl_generator_cpp::MessageInitialization _init = rosidl_generator_cpp::MessageInitialization::ALL)
  : nullbyte(_alloc)
  {
    if (rosidl_generator_cpp::MessageInitialization::ALL == _init ||
      rosidl_generator_cpp::MessageInitialization::ZERO == _init)
    {
      this->cur_yaw = 0.0;
      this->cur_pitch = 0.0;
      this->unix_time = 0.0;
      this->sentry_health = 0ll;
      this->nullbyte = "";
    }
  }

  // field types and members
  using _cur_yaw_type =
    double;
  _cur_yaw_type cur_yaw;
  using _cur_pitch_type =
    double;
  _cur_pitch_type cur_pitch;
  using _unix_time_type =
    double;
  _unix_time_type unix_time;
  using _sentry_health_type =
    int64_t;
  _sentry_health_type sentry_health;
  using _nullbyte_type =
    std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other>;
  _nullbyte_type nullbyte;

  // setters for named parameter idiom
  Type & set__cur_yaw(
    const double & _arg)
  {
    this->cur_yaw = _arg;
    return *this;
  }
  Type & set__cur_pitch(
    const double & _arg)
  {
    this->cur_pitch = _arg;
    return *this;
  }
  Type & set__unix_time(
    const double & _arg)
  {
    this->unix_time = _arg;
    return *this;
  }
  Type & set__sentry_health(
    const int64_t & _arg)
  {
    this->sentry_health = _arg;
    return *this;
  }
  Type & set__nullbyte(
    const std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other> & _arg)
  {
    this->nullbyte = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    autoaim_interface::msg::ElectricsysState_<ContainerAllocator> *;
  using ConstRawPtr =
    const autoaim_interface::msg::ElectricsysState_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<autoaim_interface::msg::ElectricsysState_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<autoaim_interface::msg::ElectricsysState_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      autoaim_interface::msg::ElectricsysState_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<autoaim_interface::msg::ElectricsysState_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      autoaim_interface::msg::ElectricsysState_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<autoaim_interface::msg::ElectricsysState_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<autoaim_interface::msg::ElectricsysState_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<autoaim_interface::msg::ElectricsysState_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__autoaim_interface__msg__ElectricsysState
    std::shared_ptr<autoaim_interface::msg::ElectricsysState_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__autoaim_interface__msg__ElectricsysState
    std::shared_ptr<autoaim_interface::msg::ElectricsysState_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const ElectricsysState_ & other) const
  {
    if (this->cur_yaw != other.cur_yaw) {
      return false;
    }
    if (this->cur_pitch != other.cur_pitch) {
      return false;
    }
    if (this->unix_time != other.unix_time) {
      return false;
    }
    if (this->sentry_health != other.sentry_health) {
      return false;
    }
    if (this->nullbyte != other.nullbyte) {
      return false;
    }
    return true;
  }
  bool operator!=(const ElectricsysState_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct ElectricsysState_

// alias to use template instance with default allocator
using ElectricsysState =
  autoaim_interface::msg::ElectricsysState_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace autoaim_interface

#endif  // AUTOAIM_INTERFACE__MSG__ELECTRICSYS_STATE__STRUCT_HPP_
