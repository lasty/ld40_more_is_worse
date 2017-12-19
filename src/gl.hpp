#pragma once

#include <string>

#include <GL/glew.h>

// #include "maths_types.hpp"

namespace GL {

void Debuging(bool enable);

//convenience GL wrapper functions

template<typename T>
void AttachAttribute(int attrib_id, size_t stride, size_t offset);

#define ATTACH_ATTRIBUTE(attrib_id, vertex, field) \
  AttachAttribute<typeof(vertex::field)>(attrib_id, sizeof(vertex), offsetof(vertex, field))

void DetachAttribute(int attrib_id);

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
