// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from autoaim_interface:msg/ElectricsysCom.idl
// generated code does not contain a copyright notice

#ifndef AUTOAIM_INTERFACE__MSG__ELECTRICSYS_COM__TRAITS_HPP_
#define AUTOAIM_INTERFACE__MSG__ELECTRICSYS_COM__TRAITS_HPP_

#include "autoaim_interface/msg/electricsys_com__struct.hpp"
#include <rosidl_generator_cpp/traits.hpp>
#include <stdint.h>
#include <type_traits>

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<autoaim_interface::msg::ElectricsysCom>()
{
  return "autoaim_interface::msg::ElectricsysCom";
}

template<>
struct has_fixed_size<autoaim_interface::msg::ElectricsysCom>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<autoaim_interface::msg::ElectricsysCom>
  : std::integral_constant<bool, false> {};

}  // namespace rosidl_generator_traits

#endif  // AUTOAIM_INTERFACE__MSG__ELECTRICSYS_COM__TRAITS_HPP_
