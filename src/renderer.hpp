#pragma once

#include <map>
#include <vector>

#include "game.hpp"
#include "shader_line.hpp"
#include "shader_textured.hpp"
#include "sprites.hpp"
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

  Shader::Textured textured_shader;


  Shader::Line line_shader;
  Shader::Line::LineVertexArray lines1;

  float oscilate = 0.0f;

  col4 white;
  col4 grey;
  col4 green;
  col4 red;

  Text font1;
  Text font2;

  VertexDataTextured text_data;
  ArrayTexture font_texture_array;

  VertexDataTextured sprite_vertexes;
  ArrayTexture sprite_texture_array;
  SpriteFactory sprite_factory;

public:
  Renderer();
  ~Renderer();

  void UseProgram(int program_id);
  void UseVAO(int vao_id);
  void EnableBlend();
  void DisableBlend();

  void InvalidateCache();

  void Resize(int width, int height);

  void DrawVertexData(const VertexDataTextured &vertex_data, int unit);

  vec2 RenderText(const Text &font, const std::string &str, const vec2 &pos, const col4 &colour);
  void RenderSprite(const Sprite &sprite, const vec2 &pos, const col4 &colour);


  void RenderPlayer(const Player &player);

  void RenderItem(const Item &item, bool colliding, bool moused_over);
  void RenderItemInfoCard(const Item &item, const vec2 &mouse_pos);

  void RenderMonster(const Monster &monster, bool moused_over);
  void RenderMonsterInfoCard(const Monster &monster, const vec2 &mouse_pos);

  void RenderProjectile(const Projectile &projectile);

  void RenderInventory(std::map<int, Item> inventory);

  void RenderGame(const GameState &state);

  void RenderAll(const Game &game);
};
