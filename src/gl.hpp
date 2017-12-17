#pragma once

#include <string>

#include <GL/glew.h>

#include "maths_types.hpp"

namespace GL {

void Debuging(bool enable);

//convenience GL wrapper functions

template<typename T>
void AttachAttribute(int attrib_id, size_t stride, size_t offset);
void DetachAttribute(int attrib_id);

#define ATTACH_ATTRIBUTE(attrib_id, v, field) \
  AttachAttribute<typeof(v::field)>(attrib_id, sizeof(v), offsetof(v, field))

int GetShaderi(int shader_id, GLenum param_name);
int GetProgrami(int program_id, GLenum param_name);
int GetInteger(GLenum param_name);
std::string GetShaderLog(int shader_id);
std::string GetProgramLog(int program_id);

int CreateShader(int shader_type, const std::string &shader_source);

void LinkProgram(int program_id);
int ValidateProgram(int program_id);

int CreateBuffers();
void DeleteBuffers(int buffer_id);

int CreateVertexArrays();
void DeleteVertexArrays(int vao_id);

bool CheckError();

} //namespace GL


template<typename T>
void GL::AttachAttribute(int attrib_id, size_t stride, size_t offset)
{
  const GLvoid *offset_ptr = reinterpret_cast<GLvoid *>(offset);

  if constexpr (std::is_same<T, vec2>::value)
  {
    glVertexAttribPointer(attrib_id, 2, GL_FLOAT, GL_FALSE, stride, offset_ptr);
  }
  else if constexpr (std::is_same<T, vec3>::value)
  {
    glVertexAttribPointer(attrib_id, 3, GL_FLOAT, GL_FALSE, stride, offset_ptr);
  }
  else if constexpr (std::is_same<T, col4>::value)
  {
    glVertexAttribPointer(attrib_id, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, offset_ptr);
  }
  else
  {
    throw "Unknown attrib type";
  }

  glEnableVertexAttribArray(attrib_id);
}
