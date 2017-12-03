
#include "to_string.hpp"

#include <sstream>

#include <SDL.h>

#include "game.hpp"
#include "gl.hpp"
#include "maths_types.hpp"


std::ostream &operator<<(std::ostream &out, vec2 const &v2)
{
  return out << "vec2{" << v2.x << ", " << v2.y << "}";
}


std::ostream &operator<<(std::ostream &out, vec3 const &v3)
{
  return out << "vec3{" << v3.x << ", " << v3.y << ", " << v3.z << "}";
}


std::ostream &operator<<(std::ostream &out, col4 const &v4)
{
  return out << "col4{" << v4.r << ", " << v4.g
             << ", " << v4.b << ", " << v4.a << "}";
}


std::ostream &operator<<(std::ostream &out, mat4 const &mat)
{
  out << "mat4 [";
  for (int x = 0; x < 4; x++)
  {
    out << "\n";
    for (int y = 0; y < 4; y++)
    {
      out << "\t" << mat.elements[y][x];
    }
  }
  out << " ]";
  return out;
}


std::string GLenum_ToString(GLenum e)
{
  switch (e)
  {

    case GL_DEBUG_SEVERITY_HIGH:
      return "SEVERITY_HIGH";
    case GL_DEBUG_SEVERITY_MEDIUM:
      return "SEVERITY_MEDIUM";
    case GL_DEBUG_SEVERITY_LOW:
      return "SEVERITY_LOW";
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      return "NOTIFICATION";
    case GL_DEBUG_TYPE_ERROR:
      return "ERROR";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      return "DEPRECATED_BEHAVIOR";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      return "UNDEFINED_BEHAVIOR";
    case GL_DEBUG_TYPE_PORTABILITY:
      return "PORTABILITY";
    case GL_DEBUG_TYPE_PERFORMANCE:
      return "PERFORMANCE";
    case GL_DEBUG_TYPE_MARKER:
      return "MARKER";
    case GL_DEBUG_TYPE_PUSH_GROUP:
      return "PUSH_GROUP";
    case GL_DEBUG_TYPE_POP_GROUP:
      return "POP_GROUP";
    case GL_DEBUG_TYPE_OTHER:
      return "OTHER";
    case GL_DEBUG_SOURCE_API:
      return "SOURCE_API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      return "SOURCE_WINDOW_SYSTEM";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return "SHADER_COMPILER";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      return "SOURCE_THIRD_PARTY";
    case GL_DEBUG_SOURCE_APPLICATION:
      return "SOURCE_APPLICATION";
    case GL_DEBUG_SOURCE_OTHER:
      return "SOURCE_OTHER";

    default:
      return "[ERROR: Unknown GLenum]";
  }
}


std::string CPPVersion()
{
  if (__cplusplus > 201703L)
    return "C++2a (experimental)";
  else if (__cplusplus >= 201703L)
    return "C++17";
  else if (__cplusplus >= 201402L)
    return "C++14";
  else if (__cplusplus >= 201103L)
    return "C++11";
  else
    return "Time to upgrade your compiler";
}


std::string GetInputName(int button_or_key)
{
  if (button_or_key <= SDL_BUTTON_X2)
  {
    switch (button_or_key)
    {
      case SDL_BUTTON_LEFT:
        return "Left Mouse";
      case SDL_BUTTON_MIDDLE:
        return "Middle Mouse";
      case SDL_BUTTON_RIGHT:
        return "Right Mouse";
      case SDL_BUTTON_X1:
        return "Mouse X1";
      case SDL_BUTTON_X2:
        return "Mouse X2";
      default:
        return "Unknown button";
    }
  }
  else
  {
    std::string keyname = SDL_GetKeyName(button_or_key);
    if (keyname.empty()) return "Unknown key or button";
    return keyname;
  }
}
