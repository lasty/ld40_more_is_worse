
#pragma once

#include "texture.hpp"
#include "maths_types.hpp"
#include "vertex.hpp"

#include <string>
#include <map>

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

  Text(std::string texture_filename, std::string font_filename);

  void ParseChar(std::string line);


  void RenderGlyph(VertexDataTextured &vertex_data, glyph g, vec2 pos);

  void RenderString(VertexDataTextured &vertex_data, const std::string str, vec2 pos);

};

