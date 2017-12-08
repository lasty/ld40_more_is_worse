
#pragma once

#include <map>
#include <string>


struct Sprite
{
  int x;
  int y;
  int w;
  int h;
};


class SpriteFactory
{
private:
  std::map<std::string, Sprite> sprite_list;

public:
  SpriteFactory();

  const Sprite &GetSprite(const std::string &sprite_name) const;
};
