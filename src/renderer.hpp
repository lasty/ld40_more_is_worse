#pragma once

#include <map>
#include <vector>

#include "game.hpp"
#include "shader.hpp"
#include "text.hpp"
#include "texture.hpp"
#include "vertex.hpp"



struct GLState
{
  int program = 0;
  int vao = 0;
  bool blending = false;
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

  float oscilate = 0.0f;

  col4 white;
  col4 grey;
  col4 green;

  Text font1;
  Text font2;

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

  void RenderPlayer(const Player &player);
  void RenderItem(const Item &item, bool colliding);

  void RenderInventory(std::map<int, Item> inventory);

  void RenderGame(const GameState &state);

  void RenderAll(const Game &game);
};
