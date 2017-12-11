
#include "renderer.hpp"

#include <cassert>
#include <sstream>
#include <vector>

#include "game.hpp"
#include "gl.hpp"
#include "maths.hpp"
#include "to_string.hpp"


///////////////////////////


Renderer::Renderer()
: lines_data(GL_DYNAMIC_DRAW)
// , particle_data(GL_DYNAMIC_DRAW, GL_TRIANGLES)
, white{1.0f, 1.0f, 1.0f, 1.0f}
, grey{0.6f, 0.6f, 0.7f, 1.0f}
, green{0.2f, 1.0f, 0.2f, 1.0f}
, red{0.9f, 0.1f, 0.2f, 1.0f}
, font1("../data/fonts/mono_0.png", "../data/fonts/mono.fnt", 0)
, font2("../data/fonts/small_0.png", "../data/fonts/small.fnt", 1)
, text_data(GL_DYNAMIC_DRAW, GL_TRIANGLES)
, font_texture_array(256, 256, 2)
, sprite_vertexes(GL_DYNAMIC_DRAW, GL_TRIANGLES)
, sprite_texture_array(512, 512, 1)
{

  font_texture_array.LoadLayer(0, "../data/fonts/mono_0.png");
  font_texture_array.LoadLayer(1, "../data/fonts/small_0.png");

  sprite_texture_array.LoadLayer(0, "../data/images/items1.xcf");

  GL::CheckError();
}


Renderer::~Renderer()
{
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


void Renderer::DrawVertexData(const VertexDataTextured &vertex_data, int unit)
{
  UseProgram(textured_shader.GetProgramId());
  UseVAO(vertex_data.GetVAO());

  textured_shader.SetOffset(0.0f, 0.0f);
  textured_shader.SetRotation(0.0f);
  textured_shader.SetZoom(1.0f);
  textured_shader.SetColour(1.0f, 1.0f, 1.0f, 1.0f);
  textured_shader.SetTexture(unit);

  vertex_data.Draw();
}


vec2 Renderer::RenderText(const Text &font, const std::string &str, const vec2 &pos, const col4 &colour)
{
  return font.RenderString(text_data, str, pos, colour);
}


void Renderer::RenderSprite(const Sprite &sprite, const vec2 &pos, const col4 &colour)
{
  vec2 pos1{pos.x - (sprite.width / 2.0f), pos.y - (sprite.height / 2.0f)};
  vec2 pos2{pos1.x + sprite.width, pos1.y + sprite.height};

  vec2 uv1{float(sprite.x), float(sprite.y)};
  vec2 uv2{float(sprite.x + sprite.width), float(sprite.y + sprite.height)};

  sprite_vertexes.DrawQuad(pos1, uv1, pos2, uv2, colour, sprite.layer);
}


std::string GetHealthText(const Health &health)
{
  std::stringstream ss;
  ss << "[ " << health.current << " / " << health.max << " ]";
  return ss.str();
}


void Renderer::RenderPlayer(const Player &player)
{
  lines_data.DrawCircle(player.position, player.radius, green);

  vec2 direction = normalize(player.direction);
  vec2 facing_circle = player.position + (direction * player.radius);
  lines_data.DrawCircle(facing_circle, player.radius / 4.0f, green);

  vec2 label_pos = player.position + vec2{-20.0f, player.radius};
  RenderText(font2, "Player", label_pos, white);
  label_pos.y += 20;
  RenderText(font2, GetHealthText(player.health), label_pos, green);
}


void Renderer::RenderItem(const Item &item, bool colliding, bool moused_over)
{
  lines_data.DrawCircle(item.position, item.radius, item.colour);

  switch (item.type)
  {
    case Item_Type::gun:
      RenderSprite(sprite_factory.GetSprite("gun"), item.position, item.colour);
      break;

    case Item_Type::health:
      RenderSprite(sprite_factory.GetSprite("healthkit"), item.position, item.colour);
      break;

    case Item_Type::command:
    case Item_Type::none:
      break;
  }

  if (colliding)
  {
    float r1 = item.radius + (oscilate * 5);
    lines_data.DrawCircle(item.position, r1, white);
  }

  if (moused_over)
  {
    float r1 = item.radius + (oscilate * 5);
    lines_data.DrawCircle(item.position, r1, white);
  }
  else
  {
    RenderText(font2, item.name, item.position + vec2{-20.0f, item.radius}, grey);
  }
}


std::string GetLimitedUsesText(const Item &item, bool inv_view)
{
  assert(item.has_limited_uses);

  std::stringstream ss;
  if (inv_view)
  {
    unsigned char mult_symbol = 215;
    ss << "  " << mult_symbol << item.uses_left;
  }
  else
  {
    ss << item.uses_left;
  }

  return ss.str();
}


std::string GetCooldownText(const Item &item, bool inv_view)
{
  assert(item.has_cooldown);

  std::stringstream ss;
  SetStreamFormat(ss);
  if (inv_view)
  {
    ss << "  [" << item.cooldown << "s]";
  }
  else
  {
    ss << item.cooldown_max << " seconds";
  }
  return ss.str();
}


void Renderer::RenderItemInfoCard(const Item &item, const vec2 &mouse_pos)
{
  vec2 infocard_pos = mouse_pos + vec2{10.0f, 20.0f};

  col4 red{1.0f, 0.2f, 0.2f, 1.0f};
  col4 tan{0.8f, 0.6f, 0.2f, 1.0f};


  RenderText(font2, item.name, infocard_pos, white);
  infocard_pos.y += 20;

  switch (item.type)
  {
    case Item_Type::health:
    {
      RenderText(font2, "Health item", infocard_pos, red);
      infocard_pos.y += 20;
      std::stringstream ss;
      ss << item.healing_amount << " healing amount";
      RenderText(font2, ss.str(), infocard_pos, grey);
      infocard_pos.y += 20;
    }
    break;


    case Item_Type::gun:
    {
      RenderText(font2, "Projectile Weapon", infocard_pos, tan);
      infocard_pos.y += 20;
      std::stringstream ss;
      ss << item.projectile_damage << " damage";
      RenderText(font2, ss.str(), infocard_pos, grey);
      infocard_pos.y += 20;
    }
    break;

    case Item_Type::command:
      RenderText(font2, "[Command]", infocard_pos, grey);
      infocard_pos.y += 20;
      break;

    case Item_Type::none:
      RenderText(font2, "[None]", infocard_pos, grey);
      infocard_pos.y += 20;
      break;
  }

  if (item.has_limited_uses)
  {
    vec2 pos2 = RenderText(font2, "Limited uses: ", infocard_pos, grey);
    pos2 = RenderText(font2, GetLimitedUsesText(item, false), pos2, green);
    infocard_pos.y += 20;
  }

  if (item.has_cooldown)
  {
    vec2 pos2 = RenderText(font2, "Cooldown: ", infocard_pos, grey);
    RenderText(font2, GetCooldownText(item, false), pos2, green);
    infocard_pos.y += 20;
  }

  //   case Active_Type::none:
  //     font2.RenderString(text_data, "[None]", infocard_pos, grey);

  //   case Active_Type::hold_down:
  //     pos2 = font2.RenderString(text_data, "Hold to use ", infocard_pos, grey);

  //   case Active_Type::limited_use:
  //     pos2 = font2.RenderString(text_data, "Limited uses ", infocard_pos, grey);

  //   case Active_Type::passive:
  //     pos2 = font2.RenderString(text_data, "Passive", infocard_pos, grey);

  //   case Active_Type::repeatable:
  //     pos2 = font2.RenderString(text_data, "Repeatable", infocard_pos, grey);

  //   case Active_Type::toggle:
  //     pos2 = font2.RenderString(text_data, "Toggle on/off ", infocard_pos, grey);
}


void Renderer::RenderMonster(const Monster &monster, bool moused_over)
{
  lines_data.DrawCircle(monster.position, monster.radius, red);

  if (moused_over)
  {
    float r1 = monster.radius + (oscilate * 5);
    lines_data.DrawCircle(monster.position, r1, white);
  }
  else
  {
    RenderText(font2, monster.name, monster.position + vec2{-20.0f, monster.radius}, grey);
  }
}


void Renderer::RenderMonsterInfoCard(const Monster &monster, const vec2 &mouse_pos)
{
  vec2 infocard_pos = mouse_pos + vec2{10.0f, 20.0f};

  RenderText(font2, monster.name, infocard_pos, white);
  infocard_pos.y += 20;


  switch (monster.type)
  {
    case Monster_Type::dummy:
      RenderText(font2, "Dummy", infocard_pos, red);
      infocard_pos.y += 20;
      break;

    case Monster_Type::melee:
      RenderText(font2, "Melee", infocard_pos, red);
      infocard_pos.y += 20;
      break;

    case Monster_Type::shooter:
      RenderText(font2, "Shooter", infocard_pos, red);
      infocard_pos.y += 20;
      break;

    case Monster_Type::none:
      break;
  }

  vec2 pos2 = RenderText(font2, "Health ", infocard_pos, grey);
  RenderText(font2, GetHealthText(monster.health), pos2, green);
  infocard_pos.y += 20;
}


void Renderer::RenderProjectile(const Projectile &projectile)
{
  lines_data.DrawCircle(projectile.position, projectile.radius, white);
}


void Renderer::RenderInventory(std::map<int, Item> inventory)
{
  vec2 pos{10.0f, 30.0f};

  RenderText(font2, "Inventory: ", pos, white);
  pos.y += 20.0f;

  for (auto & [ key, item ] : inventory)
  {
    std::stringstream ss_item;
    ss_item << GetInputName(key) << ": " << item.name;

    vec2 pos2 = RenderText(font2, ss_item.str(), pos, grey);

    if (item.has_limited_uses)
    {
      pos2 = RenderText(font2, GetLimitedUsesText(item, true), pos2, red);
    }

    if (item.has_cooldown and item.cooldown > 0.0f)
    {
      RenderText(font2, GetCooldownText(item, true), pos2, green);
    }

    pos.y += 20.0f;
  }
}


void Renderer::RenderGame(const GameState &state)
{
  oscilate = sin(state.wallclock * 5.0f);

  EnableBlend();

  lines_data.Clear();
  text_data.Clear();
  sprite_vertexes.Clear();

  for (auto &item : state.world_items)
  {
    bool colliding = state.closest_item and state.closest_item == &item;
    bool moused_over = state.mouseover_item and state.mouseover_item == &item;

    RenderItem(item, colliding, moused_over);
    if (moused_over)
    {
      RenderItemInfoCard(item, state.mouse_position);
    }
  }


  for (auto &monster : state.world_monsters)
  {
    bool moused_over = state.mouseover_monster and state.mouseover_monster == &monster;

    RenderMonster(monster, moused_over);
    if (moused_over)
    {
      RenderMonsterInfoCard(monster, state.mouse_position);
    }
  }


  for (auto &projectile : state.world_projectiles)
  {
    RenderProjectile(projectile);
  }


  RenderPlayer(state.player);


  sprite_vertexes.UpdateVertexes();
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D_ARRAY, sprite_texture_array.texture_id);
  DrawVertexData(sprite_vertexes, 1);


  lines_data.UpdateVertexes();
  DrawVertexData(GL_LINES, lines_data);

  // text_data.UpdateVertexes();
  // glActiveTexture(GL_TEXTURE0);
  // glBindTexture(GL_TEXTURE_2D_ARRAY, font_texture_array.texture_id);
  // DrawVertexData(text_data, 0);


  // text_data.Clear();

  vec2 mode_position{10.0f, 700.0f};
  vec2 mode_line2{10.0f, 730.0f};

  if (state.drop_mode)
  {
    RenderText(font1, "Drop Mode", mode_position, white);
  }
  else
  {
    RenderText(font1, "Normal Mode", mode_position, white);
  }


  // text_data.UpdateVertexes();

  // glActiveTexture(GL_TEXTURE0);
  // glBindTexture(GL_TEXTURE_2D_ARRAY, font_texture_array.texture_id);
  // DrawVertexData(text_data, 0);


  // text_data.Clear();
  if (state.drop_mode)
  {
    RenderText(font2, "Press inventory key to drop items", mode_line2, grey);
  }
  else
  {
    if (state.closest_item)
    {
      RenderText(font2, "Press a new key to pick up this item", mode_line2, grey);
    }
    else
    {
      RenderText(font2, "Move to item to pick up, or press item's key to activate", mode_line2, grey);
    }
  }

  RenderInventory(state.player.KeyBindInventory);

  text_data.UpdateVertexes();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, font_texture_array.texture_id);
  DrawVertexData(text_data, 0);
}


void Renderer::RenderAll(const Game &game)
{
  glClearColor(0.1, 0.2, 0.3, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  RenderGame(game.gamestate);

  GL::CheckError();
}
