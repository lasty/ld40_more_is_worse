
#pragma once


#include <vector>

#include "maths_types.hpp"

namespace Shader {

class Line
{
public:
  struct Vertex
  {
    vec2 position;
    col4 colour;
  };

  class VertexArray : public std::vector<Vertex>
  {
    private:
    friend class Line;
    int buffer_id;
    int vao_id;

    public:
    VertexArray();
    ~VertexArray();
    VertexArray(VertexArray &copy) = delete;

    void Update();

    void Line(const vec2 &p1, const col4 &c1, const vec2 &p2, const col4 &c2);
    void Circle(const vec2 &position, float radius, const col4 &colour);
    void Rect(vec2 position, vec2 size, col4 colour);
  };

private:
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
  Line();
  ~Line();

  void SetResolution(int width, int height);
  void SetOffset(vec2 const &offset);
  void SetRotation(float rot);
  void SetZoom(float zoom);
  void SetColour(col4 const &colour);


  void Render(VertexArray &array);

};

} //namespace Shader
