
#include "texture.hpp"

#include "gl.hpp"

#include <SDL_image.h>

#include <iostream>
#include <stdexcept>

Texture::Texture(std::string filename)
{
  glGenTextures(1, &texture_id);


  glBindTexture(GL_TEXTURE_2D, texture_id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  SDL_Surface *surf = IMG_Load(filename.c_str());
  if (not surf) throw std::runtime_error("Could not load image");

  width = surf->w;
  height = surf->h;

  int Mode = GL_RGB;
  std::cout << "width " << surf->w << "  height " << surf->h << std::endl;

  std::cout << "Bytes per pixel " << (int)surf->format->BytesPerPixel << std::endl;

  if (surf->format->BytesPerPixel == 1)
  {
    Mode = GL_RED;
  }
  if (surf->format->BytesPerPixel == 4)
  {
    Mode = GL_RGBA;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, Mode, GL_UNSIGNED_BYTE, surf->pixels);

  if (GL::CheckError()) throw std::runtime_error("glTexImage2D failed");


  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glGenerateMipmap(GL_TEXTURE_2D);
}


Texture::~Texture()
{
  glDeleteTextures(1, &texture_id);
}


