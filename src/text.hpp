
#pragma once

#include "maths_types.hpp"
#include "shader_textured.hpp"
#include "texture.hpp"

#include <codecvt>
#include <map>
#include <string>


struct glyph
{
  int x;
  int y;
  int z;
  int width;
  int height;
  int xoffset;
  int yoffset;
  int xadvance;
};


class utf8_iterator
{
public:
  utf8_iterator(const std::string &str);
  void operator++();
  char32_t operator*();
  operator bool() const;

private:
  const char *in = nullptr;
  const char *in_end = nullptr;
  const char *in_next = nullptr;

  std::codecvt_utf8<char32_t> conv;
  std::mbstate_t state;
};


class Font
{
public:
  int layer;
  float line_spacing = 0.0f;

  std::map<char32_t, glyph> glyphs;
  std::vector<std::string> image_filenames;

  Font(std::string font_filename);

  void ParseChar(std::string line);

  void RenderGlyph(Shader::Textured::VertexArray &vertex_data, glyph g, vec2 pos, const col4 &colour) const;
  vec2 RenderString(Shader::Textured::VertexArray &vertex_data, const std::string &str, vec2 pos, col4 colour) const;
};


class FontLibrary
{
public:
  FontLibrary(const std::string &font_path);

private:
  std::map<std::string, Font> fonts;
  ArrayTexture font_texture_array;

public:
  Font big;
  Font big_mono;
  Font small;
  Font small_bold;
  Font unicode;
  int max_layers = 0;

  ArrayTexture &GetTexture();
};


class TextBox
{
private:
  const Font *font = nullptr;
  Shader::Textured::VertexArray &vertex_array;

public:
  col4 colour;
  vec2 top_left;
  vec2 bot_right;
  vec2 cursor_pos;

public:
  TextBox(Shader::Textured::VertexArray &vertex_array, const Font &font, const vec2 start_pos);

  TextBox &operator<<(const Font &font);

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
