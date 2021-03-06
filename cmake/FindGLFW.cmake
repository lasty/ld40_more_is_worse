cmake_minimum_required(VERSION 3.9)

# Import library for GLFW
# link to GLFW::GLFW

find_path(GLFW_INCLUDE_DIR glfw3.h PATH_SUFFIXES GLFW)
find_library(GLFW_LIBRARY NAMES glfw3 glfw)


# find version
file(STRINGS "${GLFW_INCLUDE_DIR}/glfw3.h" GLFW_VERSION_STRING
  REGEX "^#define[ \t]+GLFW_VERSION_(MAJOR|MINOR|REVISION)")
string(REGEX REPLACE ".*([0-9]+).*([0-9]+).*([0-9]+).*" "\\1.\\2.\\3"
  GLFW_VERSION_STRING "${GLFW_VERSION_STRING}")

#message("=== GLFW_VERSION_STRING ${GLFW_VERSION_STRING}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLFW REQUIRED_VARS GLFW_LIBRARY GLFW_INCLUDE_DIR VERSION_VAR GLFW_VERSION_STRING)
mark_as_advanced(GLFW_INCLUDE_DIR GLFW_LIBRARY GLFW_VERSION_STRING)


if(GLFW_FOUND AND NOT TARGET GLFW::GLFW)
  add_library(GLFW::GLFW UNKNOWN IMPORTED)

  set_target_properties(GLFW::GLFW PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION ${GLFW_LIBRARY}
    INTERFACE_INCLUDE_DIRECTORIES ${GLFW_INCLUDE_DIR})
endif()
