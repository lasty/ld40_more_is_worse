
#include "text.hpp"

#include <cassert>
#include <fstream>

#include <iostream>
#include <sstream>


bool starts_with(std::string line, std::string key)
{
  return (line.find(key) == 0);
}


int parse(std::string line, std::string key)
{
  //   std::cout << "parsing int key '" << key << "' in string: \n"
  //             << line << std::endl;


  auto pos1 = line.find(key);
  assert(pos1 != std::string::npos);

  auto pos2 = line.find("=", pos1);
  assert(pos2 != std::string::npos);

  auto pos3 = line.find(" ", pos2);
  if (pos3 == std::string::npos) pos3 = line.size();

  std::string theint = line.substr(pos2 + 1, (pos3 - pos2));

  //   std::cout << "parsing an int: '" << theint << "'" << std::endl;
  std::stringstream ss;
  ss << theint;

  int v = -1;

  ss >> v;

  //   std::cout << "got int: " << v << std::endl;

  return v;
}


Text::Text(std::string texture_filename, std::string font_filename, int layer)
//: tex(texture_filename)
: layer(layer)
{
  //Parse font metadata

  std::ifstream in{font_filename};

  assert(in);

  std::string info_line;
  getline(in, info_line);
  assert(starts_with(info_line, "info"));

  std::string common_line;
  getline(in, common_line);
  assert(starts_with(common_line, "common"));

  std::string page_line;
  getline(in, page_line);
  assert(starts_with(page_line, "page"));

  std::string count_line;
  getline(in, count_line);
  assert(starts_with(count_line, "chars"));

  int chars_count = parse(count_line, "count");

  for (int i = 0; i < chars_count; i++)
  {
    std::string char_line;
    getline(in, char_line);
    assert(starts_with(char_line, "char"));

    ParseChar(char_line);
  }

  assert((int)glyphs.size() == chars_count);
}


void Text::ParseChar(std::string line)
{
  int id = parse(line, "id");
  int x = parse(line, "x");
  int y = parse(line, "y");
  int width = parse(line, "width");
  int height = parse(line, "height");
  int xoffset = parse(line, "xoffset");
  int yoffset = parse(line, "yoffset");
  int xadvance = parse(line, "xadvance");

  glyph g{x, y, width, height, xoffset, yoffset, xadvance};
  glyphs[id] = g;
}


void Text::RenderGlyph(VertexDataTextured &vertex_data, glyph g, vec2 pos, const col4 &colour) const
{
  vec2 pos1{pos.x + g.xoffset, pos.y + g.yoffset};
  vec2 pos2{pos1.x + g.width, pos1.y + g.height};

  vec2 uv1{float(g.x), float(g.y)};
  vec2 uv2{float(g.x + g.width), float(g.y + g.height)};

  vertex_data.DrawQuad(pos1, uv1, pos2, uv2, colour, layer);
}


vec2 Text::RenderString(VertexDataTextured &vertex_data, const std::string str, vec2 pos, col4 colour) const
{
  for (char ch : str)
  {
    auto it = glyphs.find(ch);
    if (it == glyphs.end())
    {
      pos.x += 32;
      continue;
    }

    glyph g = it->second;

    RenderGlyph(vertex_data, g, pos, colour);

    pos.x += g.xadvance;
  }

  return pos;
}
