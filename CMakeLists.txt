# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(VarSaw)
message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INSTALL_PREFIX "/Users/boonier/Music/Plogue/Bidule/plugins")

set(BIDULE_APP_NAME "Bidule 0.9791 P64" CACHE STRING "Bidule app name as seen by 'open -a'")
set(BIDULE_TEST_LAYOUT "" CACHE FILEPATH "Optional .bidule layout to open after installing")

# Define the plugin bundle name
set(PLUGIN_NAME "VarSaw")

# Define the Bidule SDK directory
set(BIDULE_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../common")

# Add the Bidule SDK source file to the plugin
add_library(${PLUGIN_NAME} MODULE
  VarSaw.cpp
  VarSawMain.cpp
  ${BIDULE_SDK_DIR}/BiduleSDK.cpp
)

# Include the Bidule SDK headers
target_include_directories(${PLUGIN_NAME} PRIVATE "${BIDULE_SDK_DIR}")

target_compile_definitions(${PLUGIN_NAME} PRIVATE MAC=1)



# Set the plugin bundle output directory
message("Set the plugin bundle output directory...")
set_target_properties(${PLUGIN_NAME} PROPERTIES
  BUNDLE ON
  BUNDLE_EXTENSION "bundle"
  BUNDLE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info-JitterNoise__Upgraded_.plist"
  # RESOURCE "Resources/en.lproj/InfoPlist.strings"
)

message("About to install plugin bundle...")
# Remove existing executable
install(CODE "
  if(EXISTS \"${CMAKE_INSTALL_PREFIX}/${PLUGIN_NAME}.bundle\")
    file(REMOVE_RECURSE \"${CMAKE_INSTALL_PREFIX}/${PLUGIN_NAME}.bundle\")
  endif()
")
install(TARGETS ${PLUGIN_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/")

add_custom_target(install_plugin
    COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}"
    COMMENT "Installing plugin bundle"
)
add_dependencies(install_plugin ${PLUGIN_NAME})

# Add custom target to launch Bidule with test layout
if(BIDULE_TEST_LAYOUT)
  add_custom_target(launch_test_layout
      COMMAND open -a "${BIDULE_APP_NAME}" "${BIDULE_TEST_LAYOUT}"
      COMMENT "Launching Bidule test layout"
  )
else()
  add_custom_target(launch_test_layout
      COMMAND open -a "${BIDULE_APP_NAME}"
      COMMENT "Launching Bidule"
  )
endif()

add_dependencies(launch_test_layout install_plugin)