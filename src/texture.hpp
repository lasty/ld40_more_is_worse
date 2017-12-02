#pragma once

#include <string>

class Texture
{
public:
  unsigned texture_id = 0;
  int texture_unit = -1;

  int width = 0;
  int height = 0;

public:
  Texture(std::string filename);
  ~Texture();
};

