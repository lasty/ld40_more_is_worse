#pragma once

#include <vector>

#include "gl.hpp"
#include "maths_types.hpp"


struct shape_def
{
  int offset;
  int count;
};


class VertexDataTextured
{
public:
  struct Vertex
  {
    vec2 position;
    col4 colour;
    vec3 uv;
  };

private:
  std::vector<Vertex> vertex_data;
  int buffer_id = 0;
  int vao_id = 0;
  GLenum usage{};
  GLenum primitive_type;

  static constexpr int stride = sizeof(Vertex);

public:
  VertexDataTextured(GLenum usage, GLenum primitive_type);
  ~VertexDataTextured();

  void Clear();

  void AddVertex(const Vertex &v);
  void AddVertex(const vec2 &position, const vec3 &uv, const col4 &colour);

  void DrawQuad(vec2 pos1, vec2 uv1, vec2 pos2, vec2 uv2, col4 colour, int layer);

  void UpdateVertexes();

  int GetVAO() const;

  void Draw() const;
  void Flush();
};
