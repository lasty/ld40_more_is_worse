
#include "text.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include <SDL_image.h>
#include <SDL_timer.h>

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


#if (LOAD_WITH_THREADS)
Font::Font(std::string font_filename, std::promise<int> num_layers_promise)
#else
Font::Font(std::string font_filename)
#endif
{
  //Parse font metadata

  std::ifstream in{font_filename};

  // std::cout << "Font() font_filename = '" << font_filename << "'" << std::endl;

  assert(in);

  std::string info_line;
  getline(in, info_line);
  assert(starts_with(info_line, "info"));

  std::string common_line;
  getline(in, common_line);
  assert(starts_with(common_line, "common"));
  line_spacing = parse(common_line, "lineHeight");
  int pages = parse(common_line, "pages");

#if (LOAD_WITH_THREADS)
  num_layers_promise.set_value(pages);
#endif

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
: font_path(font_path)
, font_texture_array(256, 256, 0)

{
  font_list.push_back("roboto_slab_18px");
  font_list.push_back("mono");
  font_list.push_back("small");
  font_list.push_back("small_bold");
  font_list.push_back("dejavu_sans_18px");

  Reload();
}


bool FontLibrary::Loaded() const
{
  return all_done;
}

#if (LOAD_WITH_THREADS)
Font make_font(std::string font_filename, std::promise<int> layer_promise)
{
  return Font(font_filename, std::move(layer_promise));
}
#endif


void FontLibrary::Reload()
{
  all_done = false;
  font_iterator = 0;
  fonts.clear();
  loading_timer = SDL_GetTicks();
  int layers = 0;

#if (LOAD_WITH_THREADS)
  std::vector<std::future<int>> layer_futures;
  layer_futures.reserve(font_list.size());

  std::map<std::string, std::future<Font>> font_load_threads;

  for (auto fontname : font_list)
  {
    std::promise<int> layer_promise;
    layer_futures.push_back(layer_promise.get_future());
    std::string font_filename = font_path + fontname + ".fnt";
    font_load_threads.emplace(fontname,
      std::async(std::launch::async, make_font, font_filename, std::move(layer_promise)));
  }

  for (auto &f : layer_futures)
  {
    layers += f.get();
  }

  std::cout << "number of layers (threaded method):  " << layers << std::endl;
  font_texture_array.ResetLayerCount(layers);


  for (auto & [ name, future ] : font_load_threads)
  {
    fonts.insert({name, future.get()});
  }
#else //LOAD_WITH_THREADS
  for (auto fontname : font_list)
  {
    std::string font_filename = font_path + fontname + ".fnt";

    fonts.emplace(fontname, Font(font_filename));
    layers += fonts.at(fontname).image_filenames.size();
  }

  std::cout << "number of layers (non-threaded method):  " << layers << std::endl;
  font_texture_array.ResetLayerCount(layers);

#endif


  image_queue.clear();
  image_queue.reserve(layers);

  layers = 0;
  for (auto &it : fonts)
  {
    auto &font = it.second;
    font.layer = layers;
    layers += font.image_filenames.size();
    image_queue.insert(image_queue.end(), font.image_filenames.begin(), font.image_filenames.end());
  }
  std::cout << "number of images (queued): " << layers << std::endl;

#if (LOAD_WITH_THREADS)
  for (unsigned i = 0; i < image_queue.size(); i++)
  {
    const auto filename = font_path + image_queue.at(i);
    // image_future f{i, std::async(std::launch::async, IMG_Load, filename.c_str())};

    image_future f{
      i, std::async(std::launch::async, [](std::string filename) {
        auto surf = IMG_Load(filename.c_str());
        //std::cout << "[img load thread, " << filename << " -> " << surf << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 5000 + 1000));
        return surf;
      },
           filename)};

    // std::cout << "async queue load  layer: " << i << "  filename: " << filename << std::endl;

    image_future_queue.push_back(std::move(f));
  }
  std::cout << "number of async image loads : " << image_future_queue.size() << std::endl;
#endif

  font_iterator = 0;
}


float FontLibrary::LoadOne()
{
  // std::cout << "[FONTS] LoadingOne()" << std::endl;
  if (image_queue.empty())
  {
    std::cout << "Warning, LoadOne called when there is no work left" << std::endl;
    return 1.0f;
  }

  // std::cout << "[FONTS] iterator is: " << font_iterator << "  image queue size is: " << image_queue.size() << std::endl;

  if (font_iterator == image_queue.size())
  {
    font_iterator = 0;
    image_queue.clear();

    auto ms = SDL_GetTicks() - loading_timer;

    std::cout << "[FONTS] All done.  Loading took " << ms << "ms"
#if (LOAD_WITH_THREADS)
              << "(threaded)"
#else
              << "(non-threaded)"
#endif
              << std::endl;


    all_done = true;
    return 1.0f;
  }

#if (LOAD_WITH_THREADS)
  for (auto it = image_future_queue.begin(); it != image_future_queue.end(); it++)
  {
    auto & [ layer, image_future ] = *it;
    if (image_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
      auto surface = image_future.get();
      if (surface)
      {
        font_texture_array.LoadLayerSurface(layer, surface);
        SDL_free(surface);
      }
      else
      {
        std::cout << "[Err] image future for layer " << layer << " return nullptr   ("
                  << SDL_GetError() << ")" << std::endl;
      }

      font_iterator++;
      image_future_queue.erase(it);
      break;
    }
  }

#else
  font_texture_array.LoadLayer(font_iterator, font_path + image_queue.at(font_iterator));
  // std::cout << "[FONTS] Load texture [" << font_iterator << "]  ->  " << font_path + image_queue.at(font_iterator) << std::endl;
  font_iterator++;
#endif


  return (float(font_iterator) / float(image_queue.size()));
}


float FontLibrary::LoadSome(unsigned ms_wait)
{
  unsigned long start_time = SDL_GetTicks();
  float f = 1.0f;

  while ((not Loaded()) and (SDL_GetTicks() - start_time) < ms_wait)
  {
    f = LoadOne();
  }
  return f;
}


void FontLibrary::ReloadAllNow()
{
  while (not Loaded())
  {
    LoadOne();
  }
}


const Font &FontLibrary::GetFont(const std::string &name) const
{
  assert(Loaded());
  return fonts.at(name);
}


ArrayTexture &FontLibrary::GetTexture()
{
  assert(Loaded());
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
