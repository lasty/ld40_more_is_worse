cmake_minimum_required(VERSION 3.9)

option(FORCE_COLOUR "Force colour diagnostics when compiling" ON)


#### Deps

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/cmake")

find_package(SDL2 2.0 REQUIRED COMPONENTS main image mixer)

find_package(GLEW 2.0 REQUIRED)

find_package(OpenGL REQUIRED)

#### System dependant shit

if(MINGW)
  set(MINGW32 mingw32)
endif()

##### Main target

add_executable(ld40 WIN32
  src/game.cpp
  src/gl.cpp
  src/main.cpp
  src/maths.cpp
  src/renderer.cpp
  src/shader.cpp
  src/sound.cpp
  src/text.cpp
  src/texture.cpp
  src/to_string.cpp
  src/vertex.cpp)


target_compile_options(ld40 PUBLIC "-std=gnu++1z")
#target_compile_features(ld40 PUBLIC cxx_std_17)


target_link_libraries(ld40 PUBLIC
  ${MINGW32}
  SDL2::SDL2 SDL2::mixer SDL2::main SDL2::image
  GLEW::GLEW
  OpenGL::GL)


#### Extra

#Enable all warnings if debug build
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Enabling all warnings")
  target_compile_options(ld40 PUBLIC
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -fmax-errors=1>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -ferror-limit=1 -Wno-missing-braces>
    )
endif()


if(FORCE_COLOUR)
  target_compile_options(ld40 PUBLIC
    $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
    $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
  )
endif()

