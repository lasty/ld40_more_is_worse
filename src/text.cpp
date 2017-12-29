
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

std::string parse_string(std::string &line, std::string key)
{
  // std::cout << "parsing string key '" << key << "' in string: \n"
  //           << line << std::endl;

  auto pos1 = line.find(key);
  assert(pos1 != std::string::npos);

  auto pos2 = line.find("=", pos1);
  assert(pos2 != std::string::npos);

  auto pos3 = line.find("\"", pos2);
  assert(pos3 != std::string::npos);

  auto pos4 = line.find("\"", pos3 + 1);
  assert(pos3 != std::string::npos);

  std::string thestring = line.substr(pos3 + 1, (pos4 - pos3) - 1);

  // std::cout << "got string: " << thestring << std::endl;

  return thestring;
}


utf8_iterator::utf8_iterator(const std::string &str)
{
  in = str.data();
  in_end = str.data() + str.size();
  in_next = in;
}


void utf8_iterator::operator++()
{
  if (in_next == in)
  {
    operator*();
  }

  in = in_next;
}


char32_t utf8_iterator::operator*()
{
  char32_t out[1];
  char32_t *out_next = nullptr;

  // std::cout << "in=" << in << "  in_end=" << in_end << "in_next=" << in_next << std::endl;

  auto result = conv.in(state, in, in_end, in_next, out, out + 1, out_next);

  // std::cout << "in=" << in << "  in_end=" << in_end << "in_next=" << in_next << std::endl;

  // std::cout << "Got char " << std::hex << out[0] << std::endl;

  if (result == std::codecvt_base::ok or result == std::codecvt_base::partial)
  {
    return out[0];
  }

  std::cout << "codecvt failed with code " << result << std::endl;

  throw std::runtime_error("codecvt failed");
}


utf8_iterator::operator bool() const
{
  return in < in_end;
}


#include <cassert>

#include <clocale>
#include <codecvt>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>


void test_utf8()
{
  using std::cout;
  using std::endl;

  std::cout << "Testing utf8 strings." << std::endl;

  std::string str0 = "";
  {
    utf8_iterator it{str0};
    assert(not it);
  }

  {
    std::string str1 = "Hello";
    utf8_iterator it{str1};
    assert(it);
    unsigned i = 0;
    while (it)
    {
      std::cout << "*it = " << *it;
      ++it;
      i++;
    }

    assert(i == str1.size());
  }

  {
    std::setlocale(LC_ALL, "utf8");

    std::string str2 = "5×9";

    utf8_iterator it{str2};
    auto c1 = *it;
    ++it;
    auto c2 = *it;
    ++it;
    auto c3 = *it;
    ++it;
    assert(c1 == '5');
    assert(c2 == 0xd7);
    assert(c3 == '9');
    assert(not it);
  }

  {
    // allow mblen() to work with UTF-8 multibyte encoding
    std::setlocale(LC_ALL, "en_US.utf8");
    // UTF-8 narrow multibyte encoding
    std::string str = //u8"z\u00df\u6c34\U0001f34c";
      u8"zß水🍌";
    // std::cout << str << " is " << str.size() << " bytes, but only "
    //           << strlen_mb(str) << " characters\n";
  }

  {
    cout << "Trying codecvt" << endl;

    std::codecvt_utf8<char32_t> conv;
    std::mbstate_t state;

    std::string str = "5×9";


    const char *from = str.data();
    const char *from_end = str.data() + str.size();
    const char *from_next = str.data();

    char32_t out[10] = {0};
    char32_t *to = out;
    char32_t *to_end = to + 10;
    char32_t *to_next = to;

    auto result = conv.in(state, from, from_end, from_next, to, to_end, to_next);

    cout << "result = " << result << endl;

    if (result == std::codecvt_base::ok)
    {
      for (int i = 0; i < 10; i++)
      {
        cout << "out[" << i << "] =  0x" << std::hex << out[i] << endl;
      }
    }
  }


  std::cout << "tests complete" << std::endl;
}


Font::Font(std::string font_filename)
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
  int pages = parse(common_line, "pages");

  for (int i = 0; i < pages; i++)
  {
    std::string page_line;
    getline(in, page_line);
    assert(starts_with(page_line, "page"));

    image_filenames.push_back(parse_string(page_line, "file"));
  }

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


void Font::ParseChar(std::string line)
{
  int id = parse(line, "id");
  int x = parse(line, "x");
  int y = parse(line, "y");
  int z = parse(line, "page");
  int width = parse(line, "width");
  int height = parse(line, "height");
  int xoffset = parse(line, "xoffset");
  int yoffset = parse(line, "yoffset");
  int xadvance = parse(line, "xadvance");

  glyph g{x, y, z, width, height, xoffset, yoffset, xadvance};
  glyphs[id] = g;
}


void Font::RenderGlyph(Shader::Textured::VertexArray &vertex_data, glyph g, vec2 pos, const col4 &colour) const
{
  vec2 pos1{pos.x + g.xoffset, pos.y + g.yoffset};
  vec2 pos2{pos1.x + g.width, pos1.y + g.height};

  vec2 uv1{float(g.x), float(g.y)};
  vec2 uv2{float(g.x + g.width), float(g.y + g.height)};

  vertex_data.DrawQuad(pos1, uv1, pos2, uv2, colour, g.z + layer);
}


vec2 Font::RenderString(Shader::Textured::VertexArray &vertex_data, const std::string &str, vec2 pos, col4 colour) const
{
  for (utf8_iterator utf_it(str); utf_it; ++utf_it)
  {
    auto ch = *utf_it;

    auto it = glyphs.find(ch);
    if (it == glyphs.end())
    {
      it = glyphs.find(-1); //try the unknown symbol glyph

      if (it == glyphs.end())
      {
        it = glyphs.find('?');

        if (it == glyphs.end())
        {
          pos.x += line_spacing;
          continue;
        }
      }
    }

    glyph g = it->second;

    RenderGlyph(vertex_data, g, pos, colour);

    pos.x += g.xadvance;
  }

  return pos;
}

///////////////////////////////////////////////////////////////////////////////


FontLibrary::FontLibrary(const std::string &font_path)
: font_texture_array(256, 256, 0)
, big(font_path + "mono.fnt")
, small(font_path + "small.fnt")
, unicode(font_path + "dejavu_sans_18px.fnt")
{
  int layers = 0;
  std::vector<std::string> image_filenames;

  const auto process_font = [&layers, &image_filenames](Font &font) {
    font.layer = layers;
    layers += font.image_filenames.size();
    image_filenames.insert(image_filenames.end(), font.image_filenames.begin(), font.image_filenames.end());
  };

  process_font(big);
  process_font(small);
  process_font(unicode);

  font_texture_array.ResetLayerCount(layers);
  for (unsigned i = 0; i < image_filenames.size(); i++)
  {
    font_texture_array.LoadLayer(i, font_path + image_filenames[i]);
  }
}


ArrayTexture &FontLibrary::GetTexture()
{
  return font_texture_array;
}


///////////////////////////////////////////////////////////////////////////////


const TextBox::endl_t TextBox::endl = {};


TextBox::TextBox(Shader::Textured::VertexArray &vertex_array, const Font &font, const vec2 start_pos)
: font(&font)
, vertex_array(vertex_array)
, colour(1.0f, 1.0f, 1.0f, 1.0f)
, top_left(start_pos)
, bot_right(start_pos)
, cursor_pos(start_pos)
{
}


TextBox &TextBox::operator<<(const Font &font)
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
