
#include "renderer.hpp"

#include <vector>

#include "game.hpp"
#include "gl.hpp"
#include "maths.hpp"
#include "to_string.hpp"

#include <SDL_image.h>


VertexDataBasic::VertexDataBasic(GLenum usage)
: usage(usage)
{
  buffer_id = GL::CreateBuffers();
  vao_id = GL::CreateVertexArrays();

  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

  AttachAttribute(0, 2, 0, GL_FLOAT); //position location = 0
  AttachAttribute(1, 4, 2, GL_FLOAT); //colour, location = 1

  glBindVertexArray(0);
}


VertexDataBasic::~VertexDataBasic()
{
  glBindVertexArray(vao_id);

  DetachAttribute(0); //position location = 0
  DetachAttribute(1); //colour, location = 1

  GL::DeleteBuffers(buffer_id);

  glBindVertexArray(0);
  GL::DeleteVertexArrays(vao_id);
}


void VertexDataBasic::Clear()
{
  vertex_data.clear();
}


void VertexDataBasic::AddVertex(const vec2 &position, const col4 &colour)
{
  vertex_data.push_back(position.x);
  vertex_data.push_back(position.y);
  vertex_data.push_back(colour.r);
  vertex_data.push_back(colour.g);
  vertex_data.push_back(colour.b);
  vertex_data.push_back(colour.a);
}


void VertexDataBasic::AddVertex(float x, float y, const col4 &colour)
{
  vertex_data.push_back(x);
  vertex_data.push_back(y);
  vertex_data.push_back(colour.r);
  vertex_data.push_back(colour.g);
  vertex_data.push_back(colour.b);
  vertex_data.push_back(colour.a);
}


void VertexDataBasic::AddLine(const vec2 &p1, const col4 &c1, const vec2 &p2, const col4 &c2)
{
  AddVertex(p1, c1);
  AddVertex(p2, c2);
}


void VertexDataBasic::UpdateVertexes()
{
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_data.size(), vertex_data.data(), usage);
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
}


int VertexDataBasic::GetOffset() const
{
  return vertex_data.size() / floats_per_vertex;
}


int VertexDataBasic::GetNumVertexes() const
{
  return vertex_data.size() / floats_per_vertex;
}


int VertexDataBasic::GetVAO() const
{
  return vao_id;
}


void VertexDataBasic::AttachAttribute(int attrib_id, int size, int offset, GLenum type)
{
  const GLvoid *offset_ptr = reinterpret_cast<GLvoid *>(offset * sizeof(float));
  // glBindVertexArray(vao_id);
  // glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glVertexAttribPointer(attrib_id, size, type, GL_FALSE, stride, offset_ptr);
  glEnableVertexAttribArray(attrib_id);
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
  // glBindVertexArray(0);
}


void VertexDataBasic::DetachAttribute(int attrib_id)
{
  // glBindVertexArray(vao_id);
  glDisableVertexAttribArray(attrib_id);
  // glBindVertexArray(0);
}


void VertexDataBasic::DrawCircle(const vec2 &position, float radius, const col4 &colour)
{
  int segments = 32;
  for (int i = 0; i < segments; i++)
    for (int j = 0; j <= 1; j++)
    {
      int a = (i + j) % segments;

      float angle = float(2 * PI) * (float(a) / float(segments));

      float x = position.x + radius * cosf(angle);
      float y = position.y + radius * sinf(angle);

      AddVertex(x, y, colour);
    }
}


// void VertexDataBasic::MakeArrow(float radius, const col4 &colour)
// {
//   std::vector<float> out;

//   float width = (PI / 8);
//   for (float angle : {0.0f, PI - width, PI + width})
//   {
//     float x = radius * cosf(angle);
//     float y = radius * sinf(angle);

//     out.push_back(x);
//     out.push_back(y);

//     out.push_back(colour.r);
//     out.push_back(colour.g);
//     out.push_back(colour.b);
//     out.push_back(colour.a);
//   }

//   return out;
// }


void VertexDataBasic::DrawRect(vec2 position, vec2 size, col4 colour)
{
  vec2 tl{position.x, position.y};
  vec2 tr{position.x + size.x, position.y};
  vec2 bl{position.x, position.y + size.y};
  vec2 br{position.x + size.x, position.y + size.y};

  AddLine(tl, colour, tr, colour);
  AddLine(tr, colour, br, colour);
  AddLine(br, colour, bl, colour);
  AddLine(bl, colour, tl, colour);
}


////////////////////


VertexDataTextured::VertexDataTextured(GLenum usage, int program_id)
: usage(usage)
, program_id(program_id)
{
  buffer_id = GL::CreateBuffers();
  vao_id = GL::CreateVertexArrays();

  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

  AttachAttribute(0, 2, 0, GL_FLOAT); //position location = 0
  AttachAttribute(1, 4, 2, GL_FLOAT); //colour, location = 1
  AttachAttribute(2, 2, 6, GL_FLOAT); //uv, location = 2

  glBindVertexArray(0);

  if (GL::CheckError()) throw std::runtime_error("VertexDataTextured() failed");
}


VertexDataTextured::~VertexDataTextured()
{
  glBindVertexArray(vao_id);

  DetachAttribute(0); //position location = 0
  DetachAttribute(1); //colour, location = 1
  DetachAttribute(2); //uv, location = 2

  GL::DeleteBuffers(buffer_id);

  glBindVertexArray(0);
  GL::DeleteVertexArrays(vao_id);
}


void VertexDataTextured::Clear()
{
  vertex_data.clear();
}

#include <cassert>

void VertexDataTextured::AddVertex(const vec2 &position, const vec2 &uv, const col4 &colour)
{
  vertex_data.push_back(position.x);
  vertex_data.push_back(position.y);
  vertex_data.push_back(colour.r);
  vertex_data.push_back(colour.g);
  vertex_data.push_back(colour.b);
  vertex_data.push_back(colour.a);
  vertex_data.push_back(uv.x);
  vertex_data.push_back(uv.y);

  assert(vertex_data.size() % floats_per_vertex == 0);
}


void VertexDataTextured::DrawQuad(vec2 pos1, vec2 uv1, vec2 pos2, vec2 uv2)
{
  col4 white{1.0f, 1.0f, 1.0f, 1.0f};

  vec2 tl{pos1.x, pos1.y};
  vec2 tr{pos2.x, pos1.y};
  vec2 bl{pos1.x, pos2.y};
  vec2 br{pos2.x, pos2.y};

  vec2 tl_uv{uv1.x, uv1.y};
  vec2 tr_uv{uv2.x, uv1.y};
  vec2 bl_uv{uv1.x, uv2.y};
  vec2 br_uv{uv2.x, uv2.y};

  AddVertex(tl, tl_uv, white);
  AddVertex(bl, bl_uv, white);
  AddVertex(tr, tr_uv, white);

  AddVertex(tr, tr_uv, white);
  AddVertex(bl, bl_uv, white);
  AddVertex(br, br_uv, white);
}


void VertexDataTextured::UpdateVertexes()
{
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_data.size(), vertex_data.data(), usage);
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
}


int VertexDataTextured::GetOffset() const
{
  return vertex_data.size() / floats_per_vertex;
}


int VertexDataTextured::GetNumVertexes() const
{
  return vertex_data.size() / floats_per_vertex;
}


int VertexDataTextured::GetVAO() const
{
  return vao_id;
}


void VertexDataTextured::AttachAttribute(int attrib_id, int size, int offset, GLenum type)
{
  const GLvoid *offset_ptr = reinterpret_cast<GLvoid *>(offset * sizeof(float));
  // glBindVertexArray(vao_id);
  // glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glVertexAttribPointer(attrib_id, size, type, GL_FALSE, stride, offset_ptr);
  glEnableVertexAttribArray(attrib_id);
  //glBindVertexArray(0);
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void VertexDataTextured::DetachAttribute(int attrib_id)
{
  // glBindVertexArray(vao_id);
  glDisableVertexAttribArray(attrib_id);
  // glBindVertexArray(0);
}


///////////////////////////


Texture::Texture(std::string filename)
{
  glGenTextures(1, &texture_id);


  glBindTexture(GL_TEXTURE_2D, texture_id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  SDL_Surface *surf = IMG_Load(filename.c_str());
  if (not surf) throw std::runtime_error("Could not load image");

  width = surf->w;
  height = surf->h;

  int Mode = GL_RGB;
  std::cout << "width " << surf->w << "  height " << surf->h << std::endl;

  std::cout << "Bytes per pixel " << (int)surf->format->BytesPerPixel << std::endl;

  if (surf->format->BytesPerPixel == 1)
  {
    Mode = GL_RED;
  }
  if (surf->format->BytesPerPixel == 4)
  {
    Mode = GL_RGBA;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, Mode, GL_UNSIGNED_BYTE, surf->pixels);

  if (GL::CheckError()) throw std::runtime_error("glTexImage2D failed");


  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glGenerateMipmap(GL_TEXTURE_2D);
}


Texture::~Texture()
{
  glDeleteTextures(1, &texture_id);
}


Text::Text(std::string texture_filename, std::string font_filename)
: tex(texture_filename)
{
}


Renderer::Renderer()
: lines_data(GL_DYNAMIC_DRAW)
, particle_data(GL_DYNAMIC_DRAW, textured_shader.GetProgramId())
, text_data(GL_DYNAMIC_DRAW, textured_shader.GetProgramId())
, white{1.0f, 1.0f, 1.0f, 1.0f}
, grey{0.5f, 0.5f, 0.5f, 1.0f}
, green{0.2f, 1.0f, 0.2f, 1.0f}
, font1("../data/fonts/mono_0.png", "../data/fonts/mono.fnt")
{
  Setup();

  GL::CheckError();
}


Renderer::~Renderer()
{
}


void Renderer::Setup()
{
  //Load textures here
}


void Renderer::UseProgram(int program_id)
{
  if (gl_state.program == program_id) return;

  gl_state.program = program_id;
  glUseProgram(program_id);
}


void Renderer::UseVAO(int vao_id)
{
  if (gl_state.vao == vao_id) return;

  gl_state.vao = vao_id;
  glBindVertexArray(vao_id);
}


void Renderer::EnableBlend()
{
  if (gl_state.blending) return;
  gl_state.blending = true;

  glEnable(GL_BLEND);
  glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
}


void Renderer::DisableBlend()
{
  if (gl_state.blending == false) return;
  gl_state.blending = false;

  glDisable(GL_BLEND);
}


void Renderer::Resize(int width, int height)
{
  basic_shader.SetResolution(width, height);
}


void Renderer::DrawVertexData(GLenum draw_type, const VertexDataBasic &vertex_data)
{
  UseProgram(basic_shader.GetProgramId());
  UseVAO(vertex_data.GetVAO());

  basic_shader.SetOffset(0.0f, 0.0f);
  basic_shader.SetRotation(0.0f);
  basic_shader.SetZoom(1.0f);
  basic_shader.SetColour(1.0f, 1.0f, 1.0f, 1.0f);

  glDrawArrays(draw_type, 0, vertex_data.GetNumVertexes());
}


void Renderer::DrawVertexData(GLenum draw_type, const VertexDataTextured &vertex_data, int unit)
{
  UseProgram(textured_shader.GetProgramId());
  UseVAO(vertex_data.GetVAO());

  textured_shader.SetOffset(0.0f, 0.0f);
  textured_shader.SetRotation(0.0f);
  textured_shader.SetZoom(1.0f);
  textured_shader.SetColour(1.0f, 1.0f, 1.0f, 1.0f);
  textured_shader.SetTexture(unit);

  // glValidateProgram(textured_shader.GetProgramId());
  // GL::CheckError();

  glDrawArrays(draw_type, 0, vertex_data.GetNumVertexes());
}


void Renderer::RenderGame(const GameState &state)
{
  // const bool draw_normals = state.debug_enabled;
  // const bool draw_velocity = state.debug_enabled;
  // const bool draw_bounds = state.debug_enabled;

  EnableBlend();
  // DisableBlend();

  lines_data.Clear();

  float r1 = 100 + (sin(state.wallclock) * 10);

  lines_data.DrawCircle({400, 200}, r1, white);
  lines_data.DrawCircle({400, 200}, 150, grey);
  //lines_data.DrawCircle({400, 200}, 200, green);

  lines_data.DrawCircle(state.player.position, 50, green);


  lines_data.UpdateVertexes();
  DrawVertexData(GL_LINES, lines_data);


  text_data.Clear();
  text_data.DrawQuad({0.0f, 0.0f}, {0.0f, 0.0f}, {256.0f, 256.0f}, {1.0f, 1.0f});
  text_data.DrawQuad({256.0f, 256.0f}, {0.0f, 0.0f}, {406.0f, 256.0f}, {1.0f, 1.0f});

  text_data.UpdateVertexes();


  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, font1.tex.texture_id);
  DrawVertexData(GL_TRIANGLES, text_data, 1);
}


void Renderer::RenderAll(const Game &game)
{
  glClearColor(0.1, 0.2, 0.3, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  RenderGame(game.gamestate);

  GL::CheckError();
}
