cmake_minimum_required(VERSION 3.9)

# Import library for GLM (OpenGl Mathematics Library)
# link to GLM::GLM


find_path(GLM_INCLUDE_DIR NAMES glm/glm.hpp)


# Find version string
file(STRINGS "${GLM_INCLUDE_DIR}/glm/detail/setup.hpp" GLM_VERSION_STRING
	REGEX "^#define[ \t]+GLM_VERSION_(MAJOR|MINOR|PATCH|REVISION)")
string(REGEX REPLACE ".*([0-9]+).*([0-9]+).*([0-9]+).*([0-9]+).*" "\\1.\\2.\\3.\\4"
GLM_VERSION_STRING "${GLM_VERSION_STRING}")


find_package(PackageHandleStandardArgs)
find_package_handle_standard_args(GLM REQUIRED_VARS GLM_INCLUDE_DIR VERSION_VAR GLM_VERSION_STRING)


if(GLM_FOUND AND NOT TARGET GLM::GLM)

	add_library(GLM::GLM UNKNOWN IMPORTED)

  set_target_properties(GLM::GLM PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    INTERFACE_COMPILE_FEATURES "cxx_std_14"
    INTERFACE_INCLUDE_DIRECTORIES ${GLM_INCLUDE_DIR}
		#INTERFACE_COMPILE_DEFINITIONS GLM_FORCE_PURE
	)

endif()
