cmake_minimum_required(VERSION 3.18)

project(FindWDK)

set(TARGET_NAME ${PROJECT_NAME})
set(MODULE_INSTALL_DIR module/)

install(FILES cmake/${PROJECT_NAME}.cmake
    DESTINATION ${MODULE_INSTALL_DIR}
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "cmake/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_PREFIX}"
    PATH_VARS MODULE_INSTALL_DIR
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" README.md LICENSE
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)
