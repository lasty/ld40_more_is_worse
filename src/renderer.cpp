
#include "renderer.hpp"

#include <sstream>
#include <vector>

#include <SDL.h>

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
, grey{0.6f, 0.6f, 0.7f, 1.0f}
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


void Renderer::RenderInventory(std::map<int, Item> inventory)
{
  vec2 pos{10.0f, 30.0f};

  font2.RenderString(text_data, "Inventory: ", pos, white);
  pos.y += 20.0f;

  for (auto & [ key, item ] : inventory)
  {
    std::stringstream ss_item;
    ss_item << SDL_GetKeyName(key) << ": " << item.name;

    vec2 pos2 = font2.RenderString(text_data, ss_item.str(), pos, grey);

    if (item.cooldown > 0.0f)
    {
      std::stringstream ss_cooldown;
      ss_cooldown.precision(1);
      ss_cooldown << std::fixed;
      ss_cooldown << " [" << item.cooldown << "s]";

      font2.RenderString(text_data, ss_cooldown.str(), pos2, green);
    }

    pos.y += 20.0f;
  }
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
  font1.SetColour(white);
  font2.SetColour(white);


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

  vec2 mode_position{10.0f, 700.0f};
  vec2 mode_line2{10.0f, 730.0f};

  if (state.drop_mode)
  {
    font1.RenderString(text_data, "Drop Mode", mode_position, white);
  }
  else
  {
    font1.RenderString(text_data, "Normal Mode", mode_position, white);
  }


  text_data.UpdateVertexes();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, font1.tex.texture_id);
  DrawVertexData(GL_TRIANGLES, text_data, 0);


  text_data.Clear();
  if (state.drop_mode)
  {
    font2.RenderString(text_data, "Press inventory key to drop items", mode_line2, grey);
  }
  else
  {
    if (state.closest_item)
    {
      font2.RenderString(text_data, "Press a new key to pick up this item", mode_line2, grey);
    }
    else
    {
      font2.RenderString(text_data, "Move to item to pick up, or press item's key to activate", mode_line2, grey);
    }
  }

  RenderInventory(state.player.KeyBindInventory);

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
