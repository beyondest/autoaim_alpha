// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from autoaim_interface:msg/CarPos.idl
// generated code does not contain a copyright notice

#ifndef AUTOAIM_INTERFACE__MSG__CAR_POS__TRAITS_HPP_
#define AUTOAIM_INTERFACE__MSG__CAR_POS__TRAITS_HPP_

#include "autoaim_interface/msg/car_pos__struct.hpp"
#include <rosidl_generator_cpp/traits.hpp>
#include <stdint.h>
#include <type_traits>

// Include directives for member types
// Member 'pose'
#include "geometry_msgs/msg/pose_stamped__traits.hpp"
// Member 'twist'
#include "geometry_msgs/msg/twist_stamped__traits.hpp"

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<autoaim_interface::msg::CarPos>()
{
  return "autoaim_interface::msg::CarPos";
}

template<>
struct has_fixed_size<autoaim_interface::msg::CarPos>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<autoaim_interface::msg::CarPos>
  : std::integral_constant<bool, false> {};

}  // namespace rosidl_generator_traits

#endif  // AUTOAIM_INTERFACE__MSG__CAR_POS__TRAITS_HPP_
