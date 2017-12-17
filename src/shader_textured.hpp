#pragma once

#include <vector>

#include "maths_types.hpp"

namespace Shader {

class Textured
{
public:
  struct Vertex
  {
    vec2 position;
    col4 colour;
    vec3 uv;
  };

  class VertexArray : public std::vector<Vertex>
  {
  public:
    VertexArray();
    ~VertexArray();

  private:
    friend class Textured;
    int buffer_id = 0;
    int vao_id = 0;

  public:
    void AddVertex(const Vertex &v);
    void AddVertex(const vec2 &position, const vec3 &uv, const col4 &colour);
    void DrawQuad(vec2 pos1, vec2 uv1, vec2 pos2, vec2 uv2, col4 colour, int layer);

    void Update();
  };

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
  void SetOffset(vec2 const &offset);
  void SetRotation(float rot);
  void SetZoom(float zoom);
  void SetColour(col4 const &colour);

  int GetProgramId() const { return program_id; }

  void SetTexture(int tex_unit);

  void Render(VertexArray &array);
};


} //namespace Shader
