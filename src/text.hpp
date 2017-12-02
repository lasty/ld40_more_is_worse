
#pragma once

#include "texture.hpp"


class Text
{
public:
  Texture tex;

  Text(std::string texture_filename, std::string font_filename);
};

