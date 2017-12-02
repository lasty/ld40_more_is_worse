#pragma once

#include <map>
#include <vector>

#include "game.hpp"
#include "shader.hpp"

typedef uint32_t GLenum;

struct GLState
{
  int program = 0;
  int vao = 0;
  bool blending = false;
};


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

  int program_id = -1;
public:
  VertexDataTextured(GLenum usage, int program_id);
  ~VertexDataTextured();

  void Clear();

  void AddVertex(const vec2 &position, const vec2 &uv, const col4 &colour);

  void DrawQuad(vec2 pos1, vec2 uv1, vec2 pos2, vec2 uv2);

  void UpdateVertexes();

  int GetOffset() const;
  int GetNumVertexes() const;

  int GetVAO() const;

  void AttachAttribute(int attrib_id, int size, int offset, GLenum type);
  void DetachAttribute(int attrib_id);
};


class Texture
{
public:
  unsigned texture_id = 0;
  int texture_unit = -1;

  int width = 0;
  int height = 0;

public:
  Texture(std::string filename);
  ~Texture();
};


class Text
{
public:
  Texture tex;

  Text(std::string texture_filename, std::string font_filename);
};


class Renderer
{
private:
  GLState gl_state;

  Shader::Basic basic_shader;
  Shader::Textured textured_shader;

  VertexDataBasic lines_data;
  VertexDataTextured particle_data;

  VertexDataTextured text_data;

  col4 white;
  col4 grey;
  col4 green;

  Text font1;

public:
  Renderer();
  ~Renderer();

  void UseProgram(int program_id);
  void UseVAO(int vao_id);
  void EnableBlend();
  void DisableBlend();

  void Resize(int width, int height);

  void DrawVertexData(GLenum draw_type, const VertexDataBasic &vertex_data);
  void DrawVertexData(GLenum draw_type, const VertexDataTextured &vertex_data, int unit);

  void Setup();

  void RenderGame(const GameState &state);

  void RenderAll(const Game &game);
};
