
#pragma once

#include "maths_types.hpp"
#include "shader_textured.hpp"
#include "texture.hpp"

#include <map>
#include <string>

struct glyph
{
  int x;
  int y;
  int width;
  int height;
  int xoffset;
  int yoffset;
  int xadvance;
};


class Text
{
public:
  int layer;
  float line_spacing = 0.0f;

  std::map<char, glyph> glyphs;

  Text(std::string font_filename, int layer);

  void ParseChar(std::string line);

  void RenderGlyph(Shader::Textured::VertexArray &vertex_data, glyph g, vec2 pos, const col4 &colour) const;
  vec2 RenderString(Shader::Textured::VertexArray &vertex_data, const std::string str, vec2 pos, col4 colour) const;
};


class TextBox
{
private:
  const Text *font = nullptr;
  Shader::Textured::VertexArray &vertex_array;

public:
  col4 colour;
  vec2 top_left;
  vec2 bot_right;
  vec2 cursor_pos;

public:
  TextBox(Shader::Textured::VertexArray &vertex_array, const Text &font, const vec2 start_pos);

  TextBox &operator<<(Text &font);

  TextBox &operator<<(const col4 &colour);

  struct endl_t
  {
  };
  const static struct endl_t endl;
  TextBox &operator<<(decltype(TextBox::endl));

  TextBox &operator<<(const std::string &s);
  TextBox &operator<<(int i);
  TextBox &operator<<(float f);

  vec2 GetSize() const;
  rect GetRect(float border = 0.0f) const;
};
