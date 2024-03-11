// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from autoaim_interface:msg/ArmorPos.idl
// generated code does not contain a copyright notice

#ifndef AUTOAIM_INTERFACE__MSG__ARMOR_POS__FUNCTIONS_H_
#define AUTOAIM_INTERFACE__MSG__ARMOR_POS__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_generator_c/visibility_control.h"
#include "autoaim_interface/msg/rosidl_generator_c__visibility_control.h"

#include "autoaim_interface/msg/armor_pos__struct.h"

/// Initialize msg/ArmorPos message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * autoaim_interface__msg__ArmorPos
 * )) before or use
 * autoaim_interface__msg__ArmorPos__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_autoaim_interface
bool
autoaim_interface__msg__ArmorPos__init(autoaim_interface__msg__ArmorPos * msg);

/// Finalize msg/ArmorPos message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_autoaim_interface
void
autoaim_interface__msg__ArmorPos__fini(autoaim_interface__msg__ArmorPos * msg);

/// Create msg/ArmorPos message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * autoaim_interface__msg__ArmorPos__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_autoaim_interface
autoaim_interface__msg__ArmorPos *
autoaim_interface__msg__ArmorPos__create();

/// Destroy msg/ArmorPos message.
/**
 * It calls
 * autoaim_interface__msg__ArmorPos__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_autoaim_interface
void
autoaim_interface__msg__ArmorPos__destroy(autoaim_interface__msg__ArmorPos * msg);


/// Initialize array of msg/ArmorPos messages.
/**
 * It allocates the memory for the number of elements and calls
 * autoaim_interface__msg__ArmorPos__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_autoaim_interface
bool
autoaim_interface__msg__ArmorPos__Sequence__init(autoaim_interface__msg__ArmorPos__Sequence * array, size_t size);

/// Finalize array of msg/ArmorPos messages.
/**
 * It calls
 * autoaim_interface__msg__ArmorPos__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_autoaim_interface
void
autoaim_interface__msg__ArmorPos__Sequence__fini(autoaim_interface__msg__ArmorPos__Sequence * array);

/// Create array of msg/ArmorPos messages.
/**
 * It allocates the memory for the array and calls
 * autoaim_interface__msg__ArmorPos__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_autoaim_interface
autoaim_interface__msg__ArmorPos__Sequence *
autoaim_interface__msg__ArmorPos__Sequence__create(size_t size);

/// Destroy array of msg/ArmorPos messages.
/**
 * It calls
 * autoaim_interface__msg__ArmorPos__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_autoaim_interface
void
autoaim_interface__msg__ArmorPos__Sequence__destroy(autoaim_interface__msg__ArmorPos__Sequence * array);

#ifdef __cplusplus
}
#endif

#endif  // AUTOAIM_INTERFACE__MSG__ARMOR_POS__FUNCTIONS_H_
