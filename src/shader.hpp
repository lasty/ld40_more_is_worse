#pragma once

#include <string>

#include "maths_types.hpp"

namespace Shader {

class Basic
{
private:
  static const std::string vertex_src;
  static const std::string fragment_src;

  int program_id = 0;
  int vertex_shader_id = 0;
  int fragment_shader_id = 0;

  struct uniform
  {
    int screen_resolution = -1;
    int offset = -1;
    int rotation = -1;
    int zoom = -1;
    int colour = -1;
  };
  uniform uniforms;

public:
  Basic();
  ~Basic();

  void SetResolution(int width, int height);

  void SetOffset(int x, int y);
  void SetOffset(vec2 const& offset);

  void SetRotation(float rot);

  void SetZoom(float zoom);

  void SetColour(float r, float g, float b, float a);
  void SetColour(col4 const& colour);


  int GetProgramId() const { return program_id; }
};


class Textured
{

private:
  static const std::string vertex_src;
  static const std::string fragment_src;

  int program_id = 0;
  int vertex_shader_id = 0;
  int fragment_shader_id = 0;

  struct uniform
  {
    int screen_resolution = -1;
    int offset = -1;
    int rotation = -1;
    int zoom = -1;
    int colour = -1;
    int texture = -1;
  };
  uniform uniforms;

public:
  Textured();
  ~Textured();

  void SetResolution(int width, int height);

  void SetOffset(int x, int y);
  void SetOffset(vec2 const& offset);

  void SetRotation(float rot);

  void SetZoom(float zoom);

  void SetColour(float r, float g, float b, float a);
  void SetColour(col4 const& colour);


  int GetProgramId() const { return program_id; }

  void SetTexture(int tex_unit);

};


} //namespace Shader
