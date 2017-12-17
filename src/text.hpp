
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

  std::map<char, glyph> glyphs;

  Text(std::string font_filename, int layer);

  void ParseChar(std::string line);

  void RenderGlyph(Shader::Textured::VertexArray &vertex_data, glyph g, vec2 pos, const col4 &colour) const;
  vec2 RenderString(Shader::Textured::VertexArray &vertex_data, const std::string str, vec2 pos, col4 colour) const;
};
