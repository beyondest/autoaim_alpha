// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from autoaim_interface:msg/DetectResult.idl
// generated code does not contain a copyright notice

#ifndef AUTOAIM_INTERFACE__MSG__DETECT_RESULT__TRAITS_HPP_
#define AUTOAIM_INTERFACE__MSG__DETECT_RESULT__TRAITS_HPP_

#include "autoaim_interface/msg/detect_result__struct.hpp"
#include <rosidl_generator_cpp/traits.hpp>
#include <stdint.h>
#include <type_traits>

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<autoaim_interface::msg::DetectResult>()
{
  return "autoaim_interface::msg::DetectResult";
}

template<>
struct has_fixed_size<autoaim_interface::msg::DetectResult>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<autoaim_interface::msg::DetectResult>
  : std::integral_constant<bool, false> {};

}  // namespace rosidl_generator_traits

#endif  // AUTOAIM_INTERFACE__MSG__DETECT_RESULT__TRAITS_HPP_
