#pragma once

#include <string>

class Texture
{
public:
  unsigned texture_id = 0;

  int width = 0;
  int height = 0;

public:
  Texture(std::string filename);
  ~Texture();
};


class ArrayTexture
{
public:
  unsigned texture_id = 0;

  int width = 0;
  int height = 0;
  int layers = 0;

public:
  ArrayTexture(int width, int height, int layers);
  ~ArrayTexture();

  void LoadLayer(int layer, std::string filename);
};