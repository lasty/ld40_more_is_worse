
#include "renderer.hpp"

#include <vector>

#include "game.hpp"
#include "gl.hpp"
#include "maths.hpp"
#include "to_string.hpp"


///////////////////////////


Renderer::Renderer()
: lines_data(GL_DYNAMIC_DRAW)
, particle_data(GL_DYNAMIC_DRAW)
, text_data(GL_DYNAMIC_DRAW)
, white{1.0f, 1.0f, 1.0f, 1.0f}
, grey{0.5f, 0.5f, 0.5f, 1.0f}
, green{0.2f, 1.0f, 0.2f, 1.0f}
, font1("../data/fonts/mono_0.png", "../data/fonts/mono.fnt")
, font2("../data/fonts/small_0.png", "../data/fonts/small.fnt")
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
  textured_shader.SetResolution(width, height);
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


void Renderer::RenderPlayer(const Player &player)
{
  lines_data.DrawCircle(player.position, player.radius, green);

  font2.RenderString(text_data, "Player", player.position + vec2{-20.0f, player.radius});
}


void Renderer::RenderItem(const Item &item, bool colliding)
{
  lines_data.DrawCircle(item.position, item.radius, item.colour);

  if (colliding)
  {
    float r1 = item.radius + (oscilate * 5);
    lines_data.DrawCircle(item.position, r1, white);
  }

  font2.RenderString(text_data, item.name, item.position + vec2{-20.0f, item.radius});
}


void Renderer::RenderGame(const GameState &state)
{
  oscilate = sin(state.wallclock * 5.0f);

  // const bool draw_normals = state.debug_enabled;
  // const bool draw_velocity = state.debug_enabled;
  // const bool draw_bounds = state.debug_enabled;

  EnableBlend();

  lines_data.Clear();
  text_data.Clear();


  for (auto &item : state.world_items)
  {
    bool colliding = state.closest_item and state.closest_item == &item;
    RenderItem(item, colliding);
  }


  RenderPlayer(state.player);


  lines_data.UpdateVertexes();
  DrawVertexData(GL_LINES, lines_data);

  text_data.UpdateVertexes();
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, font2.tex.texture_id);
  DrawVertexData(GL_TRIANGLES, text_data, 1);


  text_data.Clear();

  font1.RenderString(text_data, "Hello, World!", {100.0f, 500.0f});
  text_data.UpdateVertexes();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, font1.tex.texture_id);
  DrawVertexData(GL_TRIANGLES, text_data, 0);


  text_data.Clear();
  font2.RenderString(text_data, "Smaller Text and stuff, IIiillLLWWMM[]()!", {100.0f, 600.0f});
  text_data.UpdateVertexes();

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, font2.tex.texture_id);
  DrawVertexData(GL_TRIANGLES, text_data, 1);
}


void Renderer::RenderAll(const Game &game)
{
  glClearColor(0.1, 0.2, 0.3, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  RenderGame(game.gamestate);

  GL::CheckError();
}
