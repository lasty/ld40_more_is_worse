
#pragma once

#include "maths_types.hpp"
#include "texture.hpp"
#include "vertex.hpp"

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
  Texture tex;

  std::map<char, glyph> glyphs;

  col4 current_colour = {1.0f, 1.0f, 1.0f, 1.0f};

  Text(std::string texture_filename, std::string font_filename);

  void ParseChar(std::string line);


  void RenderGlyph(VertexDataTextured &vertex_data, glyph g, vec2 pos);

  vec2 RenderString(VertexDataTextured &vertex_data, const std::string str, vec2 pos);
  vec2 RenderString(VertexDataTextured &vertex_data, const std::string str, vec2 pos, col4 colour);

  void SetColour(col4 new_colour);
};
