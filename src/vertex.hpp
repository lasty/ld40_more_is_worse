#pragma once

#include <vector>

#include "gl.hpp"
#include "maths_types.hpp"

struct shape_def
{
  int offset;
  int count;
};


class VertexDataBasic
{
private:
  std::vector<float> vertex_data;
  int buffer_id = 0;
  int vao_id = 0;
  GLenum usage{};

  const int floats_per_vertex = 2 + 4;
  const int stride = floats_per_vertex * sizeof(float);


public:
  VertexDataBasic(GLenum usage);
  ~VertexDataBasic();

  void Clear();

  shape_def AddShape(std::vector<float> const &vertexes);

  void AddVertex(const vec2 &position, const col4 &colour);
  void AddVertex(float x, float y, const col4 &colour);
  void AddLine(const vec2 &p1, const col4 &c1, const vec2 &p2, const col4 &c2);

  void DrawCircle(const vec2 &position, float radius, const col4 &colour);
  void DrawRect(vec2 position, vec2 size, col4 colour);

  void UpdateVertexes();

  int GetOffset() const;
  int GetNumVertexes() const;

  int GetVAO() const;

  void AttachAttribute(int attrib_id, int size, int offset, GLenum type);
  void DetachAttribute(int attrib_id);
};


class VertexDataTextured
{
private:
  std::vector<float> vertex_data;
  int buffer_id = 0;
  int vao_id = 0;
  GLenum usage{};

  const int floats_per_vertex = 2 + 4 + 2;
  const int stride = floats_per_vertex * sizeof(float);

public:
  VertexDataTextured(GLenum usage);
  ~VertexDataTextured();

  void Clear();

  void AddVertex(const vec2 &position, const vec2 &uv, const col4 &colour);

  void DrawQuad(vec2 pos1, vec2 uv1, vec2 pos2, vec2 uv2);
  void DrawQuad(vec2 pos1, vec2 uv1, vec2 pos2, vec2 uv2, col4 colour);

  void UpdateVertexes();

  int GetOffset() const;
  int GetNumVertexes() const;

  int GetVAO() const;

  void AttachAttribute(int attrib_id, int size, int offset, GLenum type);
  void DetachAttribute(int attrib_id);
};
