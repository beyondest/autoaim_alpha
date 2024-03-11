// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from autoaim_interface:msg/DetectResult.idl
// generated code does not contain a copyright notice

#ifndef AUTOAIM_INTERFACE__MSG__DETECT_RESULT__STRUCT_HPP_
#define AUTOAIM_INTERFACE__MSG__DETECT_RESULT__STRUCT_HPP_

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

// Include directives for member types
// Member 'detect_result'
#include "autoaim_interface/msg/each_detect_result__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__autoaim_interface__msg__DetectResult __attribute__((deprecated))
#else
# define DEPRECATED__autoaim_interface__msg__DetectResult __declspec(deprecated)
#endif

namespace autoaim_interface
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct DetectResult_
{
  using Type = DetectResult_<ContainerAllocator>;

  explicit DetectResult_(rosidl_generator_cpp::MessageInitialization _init = rosidl_generator_cpp::MessageInitialization::ALL)
  {
    (void)_init;
  }

  explicit DetectResult_(const ContainerAllocator & _alloc, rosidl_generator_cpp::MessageInitialization _init = rosidl_generator_cpp::MessageInitialization::ALL)
  {
    (void)_init;
    (void)_alloc;
  }

  // field types and members
  using _detect_result_type =
    std::vector<autoaim_interface::msg::EachDetectResult_<ContainerAllocator>, typename ContainerAllocator::template rebind<autoaim_interface::msg::EachDetectResult_<ContainerAllocator>>::other>;
  _detect_result_type detect_result;

  // setters for named parameter idiom
  Type & set__detect_result(
    const std::vector<autoaim_interface::msg::EachDetectResult_<ContainerAllocator>, typename ContainerAllocator::template rebind<autoaim_interface::msg::EachDetectResult_<ContainerAllocator>>::other> & _arg)
  {
    this->detect_result = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    autoaim_interface::msg::DetectResult_<ContainerAllocator> *;
  using ConstRawPtr =
    const autoaim_interface::msg::DetectResult_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<autoaim_interface::msg::DetectResult_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<autoaim_interface::msg::DetectResult_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      autoaim_interface::msg::DetectResult_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<autoaim_interface::msg::DetectResult_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      autoaim_interface::msg::DetectResult_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<autoaim_interface::msg::DetectResult_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<autoaim_interface::msg::DetectResult_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<autoaim_interface::msg::DetectResult_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__autoaim_interface__msg__DetectResult
    std::shared_ptr<autoaim_interface::msg::DetectResult_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__autoaim_interface__msg__DetectResult
    std::shared_ptr<autoaim_interface::msg::DetectResult_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const DetectResult_ & other) const
  {
    if (this->detect_result != other.detect_result) {
      return false;
    }
    return true;
  }
  bool operator!=(const DetectResult_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct DetectResult_

// alias to use template instance with default allocator
using DetectResult =
  autoaim_interface::msg::DetectResult_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace autoaim_interface

#endif  // AUTOAIM_INTERFACE__MSG__DETECT_RESULT__STRUCT_HPP_
