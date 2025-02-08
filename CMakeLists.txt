cmake_minimum_required(VERSION 3.23.2)
project(FindWDK)
set(TARGET_NAME ${PROJECT_NAME})

set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")

set(INCLUDE_INSTALL_DIR include/ )

install(FILES cmake/${PROJECT_NAME}.cmake
  DESTINATION include
  COMPONENT headers
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "cmake/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_PREFIX}"
    PATH_VARS INCLUDE_INSTALL_DIR
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)
