cmake_minimum_required(VERSION 3.15)
set(PROJECT_NAME "zoom")
project(${PROJECT_NAME} LANGUAGES CXX)

# This value is used when generating builds using this plugin, so it must
# not be changed
set(PLUGIN_NAME "zoom_plugin")

add_library(${PLUGIN_NAME} SHARED
  "zoom_plugin.cpp"
)

apply_standard_settings(${PLUGIN_NAME})
set_target_properties(${PLUGIN_NAME} PROPERTIES
  CXX_VISIBILITY_PRESET hidden)

target_compile_definitions(${PLUGIN_NAME} PRIVATE FLUTTER_PLUGIN_IMPL)
target_include_directories(${PLUGIN_NAME} INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/zoom_sdk/h")

# allow the zoom sdk to have #include "fileFromZoomRoot.h" from within subdirectories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/zoom_sdk/h")
  
LINK_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/zoom_sdk/lib/") 
target_link_libraries(${PLUGIN_NAME} PRIVATE flutter flutter_wrapper_plugin "${CMAKE_CURRENT_SOURCE_DIR}/zoom_sdk/lib/sdk.lib")
#target_link_libraries(${PLUGIN_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/zoom_sdk/lib/sdk.lib")


# List of absolute paths to libraries that should be bundled with the plugin
# copy zoom sdk files
set(${PROJECT_NAME}_bundled_libraries
  "${CMAKE_CURRENT_SOURCE_DIR}/zoom_sdk/bin/"
  # FIXME: add c++ redistributables
  PARENT_SCOPE
)
#set(CMAKE_BUILD_TYPE Debug) # FIXME just for debugging