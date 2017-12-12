
#include "texture.hpp"


#include <SDL_image.h>

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "gl.hpp"
#include "texture_xcf.hpp"


Texture::Texture(std::string filename)
{
  glGenTextures(1, &texture_id);


  glBindTexture(GL_TEXTURE_2D, texture_id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  SDL_Surface *surf = IMG_Load(filename.c_str());
  if (not surf) throw std::runtime_error("Could not load image");

  width = surf->w;
  height = surf->h;

  std::cout << "width " << surf->w << "  height " << surf->h << std::endl;
  std::cout << "Bytes per pixel " << (int)surf->format->BytesPerPixel << std::endl;

  int format = surf->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;


  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, format, GL_UNSIGNED_BYTE, surf->pixels);

  if (GL::CheckError()) throw std::runtime_error("glTexImage2D failed");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenerateMipmap(GL_TEXTURE_2D);
}


Texture::~Texture()
{
  glDeleteTextures(1, &texture_id);
}


////////////////////////////////


ArrayTexture::ArrayTexture(int width, int height, int layers)
: width(width)
, height(height)
, layers(layers)
{
  glGenTextures(1, &texture_id);

  int mipmap_levels = 1;

  glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id);

  glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipmap_levels, GL_RGBA8, width, height, layers);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


ArrayTexture::~ArrayTexture()
{
  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
  glDeleteTextures(1, &texture_id);
}


void ArrayTexture::LoadLayerSurface(int layer, SDL_Surface *surf)
{
  assert(layer >= 0 and layer < layers);
  assert(surf);

  std::cout << "width " << surf->w << "  height " << surf->h << "  layer " << layer << std::endl;
  std::cout << "Bytes per pixel " << (int)surf->format->BytesPerPixel << std::endl;

  assert(surf->w == width);
  assert(surf->h == height);

  int format = surf->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;

  glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, width, height, 1, format, GL_UNSIGNED_BYTE, surf->pixels);

  if (GL::CheckError()) throw std::runtime_error("glTexSubImage3D failed");

  // glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
  // if (GL::CheckError()) throw std::runtime_error("glGenerateMipmap failed");
}


void ArrayTexture::LoadLayer(int layer, std::string filename)
{
  assert(layer >= 0 and layer < layers);

  SDL_Surface *surf = IMG_Load(filename.c_str());
  if (not surf) throw std::runtime_error("Could not load image");

  LoadLayerSurface(layer, surf);

  SDL_FreeSurface(surf);
}


void ArrayTexture::LoadLayersXCF(int layer_count, std::string filename)
{
  assert(layer_count >= 0 and layer_count <= layers);

  SDL_RWops *in = SDL_RWFromFile(filename.c_str(), "rb");
  if (not in) throw std::runtime_error("RWops thing failed");

  Sint64 start_pos = SDL_RWtell(in);

  for (int layer = 0; layer < layer_count; layer++)
  {
    SDL_RWseek(in, start_pos, RW_SEEK_SET);

    SDL_Surface *surf = IMG_Load_XCF_Layer(in, layer);
    if (not surf) throw std::runtime_error("IMG_Load_XCF_Layer returned nullptr");
    LoadLayerSurface(layer, surf);
    SDL_FreeSurface(surf);
  }

  SDL_RWclose(in);
}
