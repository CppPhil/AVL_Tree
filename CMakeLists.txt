cmake_minimum_required(VERSION 3.15)

enable_testing()

project(avl_tree_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(APP_NAME avl_tree)

set(
  HEADERS
  include/avl_tree.hpp
  include/test_framework.hpp)

set(
  SOURCES
  src/main.cpp
  src/test_framework.cpp)

add_executable(${APP_NAME} ${HEADERS} ${SOURCES})

target_include_directories(
  ${APP_NAME} 
  PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  target_compile_definitions(
    ${APP_NAME}
    PRIVATE
    DEBUG_MODE)
endif()
