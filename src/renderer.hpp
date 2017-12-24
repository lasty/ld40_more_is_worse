#pragma once

#include <map>
#include <vector>

#include "game.hpp"
#include "shader_line.hpp"
#include "shader_textured.hpp"
#include "sprites.hpp"
#include "text.hpp"
#include "texture.hpp"


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

  Shader::Line line_shader;
  Shader::Line::VertexArray lines1;

  Shader::Textured textured_shader;
  Shader::Textured::VertexArray text_data;
  Shader::Textured::VertexArray sprite_vertexes;

  float oscilate = 0.0f;

  col4 white;
  col4 grey;
  col4 green;
  col4 red;
  col4 tan;

  Text text;

  struct
  {
    const Font &big, &small, &unicode;
  } fonts;

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

  // vec2 RenderText(const Font &font, const std::string &str, const vec2 &pos, const col4 &colour);
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
