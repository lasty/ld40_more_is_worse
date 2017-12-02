cmake_minimum_required(VERSION 3.9)
#
# FindSDL2
# -------
#
# Locate SDL2 library
# Adapted from original FindSDL cmake script

# This defines an import library SDL2::SDL2

# Optional components:  main, image, mixer
# will define extra import libraries, eg, SDL2::image

# Example usage:
# find_package(SDL2 2.0 REQUIRED COMPONENTS main image)
# target_link_libraries(prog PUBLIC SDL2::main SDL2::SDL2 SDL2::image)

# Note: in MSYS/MINGW environments, link to mingw32 before SDL2::main, eg:
# add_executable(prog WIN32 myprog.cpp)
# target_link_libraries(prog PUBLIC mingw32 SDL2::main SDL2::SDL2)
# (Or alternatively just "#undef main" after you include SDL sources)


# component 'log' will enable logging/debugging this module

function(log var)
	if("log" IN_LIST SDL2_FIND_COMPONENTS)
		message("[log] ${var} = '${${var}}'")
	endif()
endfunction()


# Find include path
find_path(SDL2_INCLUDE_DIR SDL.h PATH_SUFFIXES SDL2)
log(SDL2_INCLUDE_DIR)


# SDL2 library
find_library(SDL2_LIBRARY NAMES SDL2)
log(SDL2_LIBRARY)


# Get version info
file(STRINGS "${SDL2_INCLUDE_DIR}/SDL_version.h" SDL2_VERSION_STRING REGEX "^#define[ \t]+SDL_(MAJOR|MINOR|PATCHLEVEL)")
string(REGEX REPLACE ".*([0-9]+).*([0-9]+).*([0-9]+).*" "\\1.\\2.\\3" SDL2_VERSION_STRING "${SDL2_VERSION_STRING}")
log(SDL2_VERSION_STRING)


# SDL2:SDL2
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2 REQUIRED_VARS SDL2_INCLUDE_DIR SDL2_LIBRARY VERSION_VAR SDL2_VERSION_STRING)

if(SDL2_FOUND AND NOT TARGET SDL2::SDL2)
	add_library(SDL2::SDL2 UNKNOWN IMPORTED)

  set_target_properties(SDL2::SDL2 PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION ${SDL2_LIBRARY}
    INTERFACE_INCLUDE_DIRECTORIES ${SDL2_INCLUDE_DIR})
endif()


# Handle optional components
log(SDL2_FIND_COMPONENTS)


# SDL_main library
if("main" IN_LIST SDL2_FIND_COMPONENTS)
	find_library(SDL2_MAIN_LIBRARY NAMES SDL2main)
	log(SDL2_MAIN_LIBRARY)

	find_package_handle_standard_args(SDL2_MAIN REQUIRED_VARS SDL2_MAIN_LIBRARY)

	if(SDL2_MAIN_FOUND AND NOT TARGET SDL2::main)
		add_library(SDL2::main UNKNOWN IMPORTED)

	  set_target_properties(SDL2::main PROPERTIES
	    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
	    IMPORTED_LOCATION ${SDL2_MAIN_LIBRARY})
	endif()
endif()


# SDL_Image addon library
if("image" IN_LIST SDL2_FIND_COMPONENTS)
	find_path(SDL2_IMAGE_INCLUDE_DIR SDL_image.h PATH_SUFFIXES SDL2)
	find_library(SDL2_IMAGE_LIBRARY NAMES SDL2_image)
	log(SDL2_IMAGE_INCLUDE_DIR)
	log(SDL2_IMAGE_LIBRARY)

	find_package_handle_standard_args(SDL2_IMAGE REQUIRED_VARS SDL2_IMAGE_LIBRARY SDL2_IMAGE_INCLUDE_DIR)

	if(SDL2_IMAGE_FOUND AND NOT TARGET SDL2::image)

		add_library(SDL2::image UNKNOWN IMPORTED)
	  set_target_properties(SDL2::image PROPERTIES
	    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
	    IMPORTED_LOCATION ${SDL2_IMAGE_LIBRARY}
	    INTERFACE_INCLUDE_DIRECTORIES ${SDL2_IMAGE_INCLUDE_DIR})

		#target_compile_definitions(SDL2::image INTERFACE USE_SDL2_IMAGE)
		#set(USE_SDL2_IMAGE YES)
	endif()
endif()


# SDL_Mixer addon library
if("mixer" IN_LIST SDL2_FIND_COMPONENTS)
	find_path(SDL2_MIXER_INCLUDE_DIR SDL_mixer.h PATH_SUFFIXES SDL2)
	find_library(SDL2_MIXER_LIBRARY NAMES SDL2_mixer)
	log(SDL2_MIXER_INCLUDE_DIR)
	log(SDL2_MIXER_LIBRARY)

	find_package_handle_standard_args(SDL2_MIXER REQUIRED_VARS SDL2_MIXER_LIBRARY SDL2_MIXER_INCLUDE_DIR)

	if(SDL2_MIXER_FOUND AND NOT TARGET SDL2::mixer)
		add_library(SDL2::mixer UNKNOWN IMPORTED)
	  set_target_properties(SDL2::mixer PROPERTIES
	    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
	    IMPORTED_LOCATION ${SDL2_MIXER_LIBRARY}
	    INTERFACE_INCLUDE_DIRECTORIES ${SDL2_MIXER_INCLUDE_DIR})

		#target_compile_definitions(SDL2::mixer INTERFACE USE_SDL2_MIXER)
		#set(USE_SDL2_MIXER YES)
	endif()
endif()


# TODO: Add more addon libraries
