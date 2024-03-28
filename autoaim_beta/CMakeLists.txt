cmake_minimum_required(VERSION 3.10)


set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${ROOT_DIR}/build/exe)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ROOT_DIR}/lib)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(USE_EIGEN ON)


project(autoaim_beta CXX)

include(FindTensorRT.cmake)
find_package(OpenCV REQUIRED)
#find_package(Eigen3 REQUIRED)
find_package(rclcpp REQUIRED)
find_package(autoaim_interface REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(spdlog REQUIRED)
#find_package(TensorRT REQUIRED)

set(lib_list)
set(header_list)
set(inc_list)

file(GLOB_RECURSE header_list ${ROOT_DIR}/inc/*.h ${ROOT_DIR}/src/*.h ${ROOT_DIR}/libsrc/*.h)
file(GLOB_RECURSE header_list ${ROOT_DIR}/inc/*.hpp ${ROOT_DIR}/src/*.hpp ${ROOT_DIR}/libsrc/*.hpp)
file(GLOB lib_list ${ROOT_DIR}/lib/*)


foreach(header_file ${header_list})
    get_filename_component(header_dir ${header_file} DIRECTORY)
    list(APPEND inc_list ${header_dir} " ")
endforeach(header_file ${header_list})

list(APPEND inc_list /usr/include " ")
list(APPEND inc_list /usr/local/include " ")
list(APPEND inc_list ${ROOT_DIR}/inc/gg " ")
#list(APPEND inc_list /usr/local/incluse/opencv4 " ")
#list(APPEND inc_list /usr/local/incluse/opencv4/opencv2 " ")

#list(REMOVE_DUPLICATES inc_list)


list(APPEND lib_list /usr/local/lib/libyaml-cpp.a
                     /lib/libMVSDK.so
                     /lib/libgxiapi.so
                     /lib/)

include_directories(${inc_list} PUBLIC 
                                ${OpenCV_INCLUDE_DIRS} 
                                ${spdlog_INCLUDE_DIRS}
                                ${TensorRT_INCLUDE_DIRS} )






add_compile_options(-Wall -Wextra -Wpedantic -Wno-deprecated-declarations -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-g)
else()
  add_compile_options(-O3)
endif()



add_executable(node_webcam ${ROOT_DIR}/src/node_webcam.cpp
                              ${ROOT_DIR}/src/basic.cpp
                              ${ROOT_DIR}/src/detector.cpp
                              ${ROOT_DIR}/src/mv_class.cpp
                              ${ROOT_DIR}/src/trt_module.cpp
                              ${ROOT_DIR}/src/gg_class.cpp
                              ${TensorRT_SOURCE})
ament_target_dependencies(node_webcam rclcpp autoaim_interface sensor_msgs cv_bridge)
target_link_libraries(node_webcam ${OpenCV_LIBS} 
                                     ${lib_list} 
                                     ${spdlog_LIBRARIES}
                                     ${TensorRT_LIBS}
                                     nvonnxparser
                                     cuda)

add_executable(config_all2 ${ROOT_DIR}/src/config_all.cpp
                              ${ROOT_DIR}/src/basic.cpp
                              ${ROOT_DIR}/src/detector.cpp
                              ${ROOT_DIR}/src/mv_class.cpp
                              ${ROOT_DIR}/src/trt_module.cpp
                              ${TensorRT_SOURCE})
ament_target_dependencies(config_all2 rclcpp autoaim_interface sensor_msgs cv_bridge)
target_link_libraries(config_all2 ${OpenCV_LIBS} 
                                     ${lib_list} 
                                     ${spdlog_LIBRARIES}
                                     ${TensorRT_LIBS}
                                     nvonnxparser
                                     cuda)
                                     
install(
    TARGETS 
    node_webcam
    DESTINATION lib/${PROJECT_NAME}
)




ament_package()

