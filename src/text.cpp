
#include "text.hpp"

#include <cassert>
#include <fstream>

#include <iostream>
#include <sstream>

#include "maths.hpp"


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


Text::Text(std::string font_filename, int layer)
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
  line_spacing = parse(common_line, "lineHeight");

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


void Text::RenderGlyph(Shader::Textured::VertexArray &vertex_data, glyph g, vec2 pos, const col4 &colour) const
{
  vec2 pos1{pos.x + g.xoffset, pos.y + g.yoffset};
  vec2 pos2{pos1.x + g.width, pos1.y + g.height};

  vec2 uv1{float(g.x), float(g.y)};
  vec2 uv2{float(g.x + g.width), float(g.y + g.height)};

  vertex_data.DrawQuad(pos1, uv1, pos2, uv2, colour, layer);
}


vec2 Text::RenderString(Shader::Textured::VertexArray &vertex_data, const std::string str, vec2 pos, col4 colour) const
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

const TextBox::endl_t TextBox::endl = {};

TextBox::TextBox(Shader::Textured::VertexArray &vertex_array, const Text &font, const vec2 start_pos)
: font(&font)
, vertex_array(vertex_array)
, colour(1.0f, 1.0f, 1.0f, 1.0f)
, top_left(start_pos)
, bot_right(start_pos)
, cursor_pos(start_pos)
{
}


TextBox &TextBox::operator<<(Text &font)
{
  this->font = &font;
  return *this;
}


TextBox &TextBox::operator<<(const col4 &colour)
{
  this->colour = colour;
  return *this;
}


TextBox &TextBox::operator<<(decltype(TextBox::endl))
{
  cursor_pos.x = top_left.x;
  cursor_pos.y += font->line_spacing;
  return *this;
}


TextBox &TextBox::operator<<(const std::string &s)
{
  cursor_pos = font->RenderString(vertex_array, s, cursor_pos, colour);

  bot_right.x = std::max(bot_right.x, cursor_pos.x);
  bot_right.y = std::max(bot_right.y, cursor_pos.y + font->line_spacing);

  return *this;
}


TextBox &TextBox::operator<<(int i)
{
  std::stringstream ss;
  ss << i;
  return operator<<(ss.str());
}


TextBox &TextBox::operator<<(float f)
{
  std::stringstream ss;
  ss.precision(1);
  ss << std::fixed;
  ss << f;
  return operator<<(ss.str());
}


vec2 TextBox::GetSize() const
{
  return bot_right - top_left;
}


rect TextBox::GetRect(float border) const
{
  rect r = {top_left, GetSize()};
  if (border != 0.0f)
  {
    return grow_rect(r, border);
  }
  return r;
}
