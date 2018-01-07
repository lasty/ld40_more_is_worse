
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
: white{1.0f, 1.0f, 1.0f, 1.0f}
, grey{0.6f, 0.6f, 0.7f, 1.0f}
, green{0.2f, 1.0f, 0.2f, 1.0f}
, red{0.9f, 0.1f, 0.2f, 1.0f}
, tan{0.8f, 0.6f, 0.2f, 1.0f}
, fonts("../data/fonts/")
, font_infocard_title(fonts.small_bold)
, font_infocard_body(fonts.small)
, sprite_texture_array(512, 512, 5)
{
  sprite_texture_array.LoadLayersXCF(5, "../data/images/items1.xcf");

  GL::CheckError();
}


Renderer::~Renderer()
{
  GL::CheckError();

  glUseProgram(0);
  glBindVertexArray(0);

  GL::CheckError();
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

void Renderer::InvalidateCache()
{
  gl_state = {};
}

void Renderer::Resize(int width, int height)
{
  resolution.x = width;
  resolution.y = height;

  line_shader.SetResolution(width, height);
  textured_shader.SetResolution(width, height);
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
  lines1.Circle(player.position, player.radius, green);

  vec2 direction = normalize(player.direction);
  vec2 facing_circle = player.position + (direction * player.radius);
  lines1.Circle(facing_circle, player.radius / 4.0f, green);

  TextBox box{text_data, fonts.small, player.position + vec2{-20.0f, player.radius}};
  box << white << "Player" << box.endl
      << green << GetHealthText(player.health);
}


void Renderer::RenderItem(const Item &item, bool colliding, bool moused_over)
{
  lines1.Circle(item.position, item.radius, item.colour);

  if (not item.animation.empty())
  {
    auto anim = sprite_factory.GetAnimation(item.animation);

    auto sprite = anim.GetFrame(item.animation_time);
    RenderSprite(sprite, item.position, item.colour);
  }
  else
    switch (item.type)
    {
      case Item_Type::gun:
        RenderSprite(sprite_factory.GetSprite("gun_shadow"), item.position, white);
        RenderSprite(sprite_factory.GetSprite("gun_base"), item.position, white);
        RenderSprite(sprite_factory.GetSprite("gun_top"), item.position, item.colour);
        break;

      case Item_Type::health:
        RenderSprite(sprite_factory.GetSprite("healthkit_shadow"), item.position, white);
        RenderSprite(sprite_factory.GetSprite("healthkit_base"), item.position, white);
        RenderSprite(sprite_factory.GetSprite("healthkit_top"), item.position, item.colour);
        break;

      case Item_Type::command:
      case Item_Type::none:
        break;
    }

  if (colliding)
  {
    float r1 = item.radius + (oscilate * 5);
    lines1.Circle(item.position, r1, white);
  }

  if (moused_over)
  {
    float r1 = item.radius + (oscilate * 5);
    lines1.Circle(item.position, r1, white);
  }
  else
  {
    TextBox box(text_data, fonts.small, item.position + vec2{-20.0f, item.radius});
    box << grey << item.name;
  }
}


std::string GetLimitedUsesText(const Item &item, bool inv_view)
{
  assert(item.has_limited_uses);

  std::stringstream ss;
  if (inv_view)
  {
    ss << " ";
  }

  ss << u8" ×" << item.uses_left;

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

  TextBox box(text_data, font_infocard_title, infocard_pos);

  box << white << item.name << box.endl
      << font_infocard_body;

  switch (item.type)
  {
    case Item_Type::health:
    {
      box << red << "Health item" << box.endl;

      box << grey << item.healing_amount << " healing amount" << box.endl;
    }
    break;


    case Item_Type::gun:
    {
      box << tan << "Projectile Weapon" << box.endl;

      box << grey << item.projectile_damage << " damage" << box.endl;
    }
    break;

    case Item_Type::command:
      box << grey << "[Command]" << box.endl;
      break;

    case Item_Type::none:
      box << grey << "[None]" << box.endl;
  }

  if (item.has_limited_uses)
  {
    box << grey << "Limited uses: " << green << GetLimitedUsesText(item, false) << box.endl;
  }

  if (item.has_cooldown)
  {
    box << grey << "Cooldown: " << green << GetCooldownText(item, false) << box.endl;
  }

  //   case Active_Type::none:
  //     font2.RenderString(text_data, "[None]", infocard_pos, grey);

  //   case Active_Type::hold_down:
  //     pos2 = font2.RenderString(text_data, "Hold to use ", infocard_pos, grey);

  //   case Active_Type::passive:
  //     pos2 = font2.RenderString(text_data, "Passive", infocard_pos, grey);

  //   case Active_Type::toggle:
  //     pos2 = font2.RenderString(text_data, "Toggle on/off ", infocard_pos, grey);

  if (true)
  {
    box << grey << "Animation: '" << item.animation << "'";

    float fm = fmod(item.animation_time, 1.0f);
    int im = fm * 3;
    box << green << "[" << item.animation_time << "]  fmod(" << fm << ") ->int = " << im << box.endl;
  }

  auto[box_topleft, box_size] = box.GetRect(5.0f);

  lines1.Rect(box_topleft, box_size, grey);
}


void Renderer::RenderMonster(const Monster &monster, bool moused_over)
{
  lines1.Circle(monster.position, monster.radius, red);

  if (moused_over)
  {
    float r1 = monster.radius + (oscilate * 5);
    lines1.Circle(monster.position, r1, white);
  }
  else
  {
    TextBox box(text_data, fonts.small, monster.position + vec2{-20.0f, monster.radius});
    box << grey << monster.name;
  }
}


void Renderer::RenderMonsterInfoCard(const Monster &monster, const vec2 &mouse_pos)
{
  vec2 infocard_pos = mouse_pos + vec2{10.0f, 20.0f};

  TextBox box(text_data, font_infocard_title, infocard_pos);

  box << white << monster.name << box.endl
      << font_infocard_body;

  switch (monster.type)
  {
    case Monster_Type::dummy:
      box << red << "Dummy" << box.endl;
      break;

    case Monster_Type::melee:
      box << red << "Melee" << box.endl;
      break;

    case Monster_Type::shooter:
      box << red << "Shooter" << box.endl;
      break;

    case Monster_Type::none:
      break;
  }

  box << grey << "Health " << green << GetHealthText(monster.health) << box.endl;

  auto[box_topleft, box_size] = box.GetRect(5.0f);

  lines1.Rect(box_topleft, box_size, red);
}


void Renderer::RenderProjectile(const Projectile &projectile)
{
  lines1.Circle(projectile.position, projectile.radius, white);
}


void Renderer::RenderInventory(std::map<int, Item> inventory)
{
  TextBox box(text_data, font_infocard_title, {10.0f, 30.0f});

  box << white << "Inventory:" << box.endl
      << font_infocard_body;

  for (auto & [ key, item ] : inventory)
  {
    box << grey << GetInputName(key) << ": " << item.name;

    if (item.has_limited_uses)
    {
      box << red << GetLimitedUsesText(item, true);
    }

    if (item.has_cooldown and item.cooldown > 0.0f)
    {
      box << green << GetCooldownText(item, true);
    }

    box << box.endl;
  }

  auto[box_topleft, box_size] = box.GetRect(5.0f);
  lines1.Rect(box_topleft, box_size, grey);
}


void Renderer::RenderGame(const GameState &state)
{
  oscilate = sin(state.wallclock * 5.0f);

  lines1.clear();
  text_data.clear();
  sprite_vertexes.clear();

  // lines1.Line({150, 150}, red, {500, 500}, green);


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


  vec2 mode_position{10.0f, 600.0f};
  TextBox box(text_data, fonts.big, mode_position);

  box << fonts.big << white;

  if (state.drop_mode)
  {
    box << "Drop Mode";
  }
  else
  {
    box << "Normal Mode";
  }

  box << box.endl
      << fonts.small << grey;

  if (state.drop_mode)
  {
    box << "Press inventory key to drop items";
  }
  else
  {
    if (state.closest_item)
    {
      box << "Press a new key to pick up this item";
    }
    else
    {
      box << "Move to item to pick up, or press item's key to activate";
    }
  }


  // auto qbf = "The© quick brown fox™ jumps over the lazy dog.";
  // box << white << box.endl
  //     << fonts.big << qbf << box.endl
  //     << fonts.small_bold << qbf << box.endl
  //     << fonts.small << qbf << box.endl
  //     << fonts.small2 << qbf << box.endl
  //     << fonts.unicode << qbf << box.endl;


  RenderInventory(state.player.KeyBindInventory);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D_ARRAY, sprite_texture_array.texture_id);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, fonts.GetTexture().texture_id);

  EnableBlend();

  sprite_vertexes.Update();
  textured_shader.SetTexture(1);
  textured_shader.Render(sprite_vertexes);


  lines1.Update();
  line_shader.Render(lines1);


  text_data.Update();
  textured_shader.SetTexture(0);
  textured_shader.Render(text_data);


  InvalidateCache();
}


void Renderer::RenderAll(const Game &game)
{
  if (game.debug.flag1) return RenderProgressBar(0.2f);
  if (game.debug.flag2) return RenderProgressBar(0.5f);


  glClearColor(0.1, 0.2, 0.3, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  // font_infocard_body = game.debug.flag1 ? fonts.small2 : fonts.small;
  // font_infocard_title = game.debug.flag2 ? fonts.small_serif : fonts.small_bold;

  RenderGame(game.gamestate);

  GL::CheckError();
}


void Renderer::RenderProgressBar(float v)
{
  glClearColor(0.1, 0.2, 0.3, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  col4 black{0.0f, 0.0f, 0.0f, 1.0f};

  float bar_width = 500;
  float bar_height = 10;
  float border = 5;

  vec2 middle_screen = resolution / 2.0f;

  vec2 l1 = middle_screen - vec2{bar_width / 2.0f, 0};
  vec2 l2 = middle_screen + vec2{bar_width / 2.0f, 0};

  vec2 progress = ((l2 - l1) * v) + l1;

  rect r{{l1.x - border, l1.y - border}, {bar_width + border * 2.0f, bar_height + border * 2.0f}};


  lines1.clear();

  lines1.Rect(r.position, r.size, white);

  vec2 inc;
  for (float i = 0; i <= bar_height; i += 1, inc.y += 1.0f)
  {
    //    lines1.Line(l1 + inc, black, l2 + inc, black);
    lines1.Line(l1 + inc, grey, progress + inc, grey);
  }

  lines1.Update();
  line_shader.Render(lines1);

  GL::CheckError();
}