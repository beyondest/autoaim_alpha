// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from autoaim_interface:msg/ArmorPos.idl
// generated code does not contain a copyright notice

#ifndef AUTOAIM_INTERFACE__MSG__ARMOR_POS__STRUCT_HPP_
#define AUTOAIM_INTERFACE__MSG__ARMOR_POS__STRUCT_HPP_

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
// Member 'pose'
#include "geometry_msgs/msg/pose_stamped__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__autoaim_interface__msg__ArmorPos __attribute__((deprecated))
#else
# define DEPRECATED__autoaim_interface__msg__ArmorPos __declspec(deprecated)
#endif

namespace autoaim_interface
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct ArmorPos_
{
  using Type = ArmorPos_<ContainerAllocator>;

  explicit ArmorPos_(rosidl_generator_cpp::MessageInitialization _init = rosidl_generator_cpp::MessageInitialization::ALL)
  : pose(_init)
  {
    if (rosidl_generator_cpp::MessageInitialization::ALL == _init ||
      rosidl_generator_cpp::MessageInitialization::ZERO == _init)
    {
      this->armor_name = "";
      this->armor_id = 0ll;
      this->confidence = 0.0;
    }
  }

  explicit ArmorPos_(const ContainerAllocator & _alloc, rosidl_generator_cpp::MessageInitialization _init = rosidl_generator_cpp::MessageInitialization::ALL)
  : armor_name(_alloc),
    pose(_alloc, _init)
  {
    if (rosidl_generator_cpp::MessageInitialization::ALL == _init ||
      rosidl_generator_cpp::MessageInitialization::ZERO == _init)
    {
      this->armor_name = "";
      this->armor_id = 0ll;
      this->confidence = 0.0;
    }
  }

  // field types and members
  using _armor_name_type =
    std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other>;
  _armor_name_type armor_name;
  using _armor_id_type =
    int64_t;
  _armor_id_type armor_id;
  using _confidence_type =
    double;
  _confidence_type confidence;
  using _pose_type =
    geometry_msgs::msg::PoseStamped_<ContainerAllocator>;
  _pose_type pose;

  // setters for named parameter idiom
  Type & set__armor_name(
    const std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other> & _arg)
  {
    this->armor_name = _arg;
    return *this;
  }
  Type & set__armor_id(
    const int64_t & _arg)
  {
    this->armor_id = _arg;
    return *this;
  }
  Type & set__confidence(
    const double & _arg)
  {
    this->confidence = _arg;
    return *this;
  }
  Type & set__pose(
    const geometry_msgs::msg::PoseStamped_<ContainerAllocator> & _arg)
  {
    this->pose = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    autoaim_interface::msg::ArmorPos_<ContainerAllocator> *;
  using ConstRawPtr =
    const autoaim_interface::msg::ArmorPos_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<autoaim_interface::msg::ArmorPos_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<autoaim_interface::msg::ArmorPos_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      autoaim_interface::msg::ArmorPos_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<autoaim_interface::msg::ArmorPos_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      autoaim_interface::msg::ArmorPos_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<autoaim_interface::msg::ArmorPos_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<autoaim_interface::msg::ArmorPos_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<autoaim_interface::msg::ArmorPos_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__autoaim_interface__msg__ArmorPos
    std::shared_ptr<autoaim_interface::msg::ArmorPos_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__autoaim_interface__msg__ArmorPos
    std::shared_ptr<autoaim_interface::msg::ArmorPos_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const ArmorPos_ & other) const
  {
    if (this->armor_name != other.armor_name) {
      return false;
    }
    if (this->armor_id != other.armor_id) {
      return false;
    }
    if (this->confidence != other.confidence) {
      return false;
    }
    if (this->pose != other.pose) {
      return false;
    }
    return true;
  }
  bool operator!=(const ArmorPos_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct ArmorPos_

// alias to use template instance with default allocator
using ArmorPos =
  autoaim_interface::msg::ArmorPos_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace autoaim_interface

#endif  // AUTOAIM_INTERFACE__MSG__ARMOR_POS__STRUCT_HPP_
