
#include "renderer.hpp"

#include <vector>

#include "game.hpp"
#include "gl.hpp"
#include "maths.hpp"
#include "to_string.hpp"


VertexDataBasic::VertexDataBasic(GLenum usage)
: usage(usage)
{
  buffer_id = GL::CreateBuffers();
  vao_id = GL::CreateVertexArrays();

  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

  AttachAttribute(0, 2, 0, GL_FLOAT); //position location = 0
  AttachAttribute(1, 4, 2, GL_FLOAT); //colour, location = 1
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


VertexDataTextured::VertexDataTextured(GLenum usage)
{
}


VertexDataTextured::~VertexDataTextured()
{
}


Renderer::Renderer()
: lines_data(GL_DYNAMIC_DRAW)
, particle_data(GL_DYNAMIC_DRAW)
, white{1.0f, 1.0f, 1.0f, 1.0f}
, grey{0.5f, 0.5f, 0.5f, 1.0f}
, green{0.2f, 1.0f, 0.2f, 1.0f}
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
  //glEnablei(GL_BLEND, 0);
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


void Renderer::RenderGame(const GameState &state)
{
  // const bool draw_normals = state.debug_enabled;
  // const bool draw_velocity = state.debug_enabled;
  // const bool draw_bounds = state.debug_enabled;

  EnableBlend();

  lines_data.Clear();

  float r1 = 100 + (sin(state.wallclock) * 10);

  lines_data.DrawCircle({400, 200}, r1, white);
  lines_data.DrawCircle({400, 200}, 150, grey);
  //lines_data.DrawCircle({400, 200}, 200, green);

  lines_data.DrawCircle(state.player.position, 50, green);


  lines_data.UpdateVertexes();
  DrawVertexData(GL_LINES, lines_data);
}


void Renderer::RenderAll(const Game &game)
{
  glClearColor(0.1, 0.2, 0.3, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  RenderGame(game.gamestate);

  GL::CheckError();
}
