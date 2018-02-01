#pragma once

#include <future>
#include <memory>
#include <vector>

#include "SDL_image.h"
#include "texture.hpp"

class Task
{
public:
  virtual ~Task() = default;
  virtual bool Done() const = 0;
  virtual void GLSetup() = 0;
};


class ImageLoadTask : public Task
{
public:
  ImageLoadTask(std::string filename, ArrayTexture& texture, int layer);
  ImageLoadTask(const ImageLoadTask& copy) = delete;
  ImageLoadTask(ImageLoadTask&& move) = delete;

private:
  std::string filename;
  ArrayTexture& texture;
  int layer;

  std::future<SDL_Surface*> future_surface;

public:
  bool Done() const;

  static SDL_Surface* LoadImage(std::string filename);

  void GLSetup();
};


class TaskManager
{
public:
  TaskManager();

private:
  std::vector<std::unique_ptr<Task>> task_list;
  int total_tasks = 0;

public:
  void ResetProgress();
  void AddTask(Task* new_task);

  bool Done() const;
  float Progress() const;
  //   unsigned GetSize() const { return task_list.size(); }

  float ProcessSome(unsigned ms_wait);
};
