
#include "tasks.hpp"

#include <cassert>
#include <iostream>

/////////////////////////////////////


ImageLoadTask::ImageLoadTask(std::string filename, ArrayTexture& texture, int layer)
: filename(filename)
, texture(texture)
, layer(layer)
{
  future_surface = std::async(std::launch::async, IMG_Load, filename.c_str());
}


bool ImageLoadTask::Done() const
{
  auto status = future_surface.wait_for(std::chrono::seconds(0));
  return status == std::future_status::ready;
}


void ImageLoadTask::GLSetup()
{
  assert(Done());

  auto surface = future_surface.get();

  texture.LoadLayerSurface(layer, surface);

  SDL_FreeSurface(surface);
}


/////////////////////////////////////


TaskManager::TaskManager()
{
  ResetProgress();
}


void TaskManager::ResetProgress()
{
  assert(task_list.empty());
  total_tasks = 0;
}


void TaskManager::AddTask(Task* new_task)
{
  task_list.emplace_back(new_task);
  total_tasks++;
}


bool TaskManager::Done() const
{
  return task_list.empty();
}


float TaskManager::Progress() const
{
  if (Done()) return 1.0f;

  assert(total_tasks != 0);
  int processed_tasks = total_tasks - task_list.size();
  assert(processed_tasks >= 0 and processed_tasks <= total_tasks);
  return float(processed_tasks) / float(total_tasks);
}


float TaskManager::ProcessSome(unsigned ms_wait)
{
  unsigned long start_time = SDL_GetTicks();

  while ((not Done()) and (SDL_GetTicks() - start_time) < ms_wait)
  {
    for (auto it = task_list.begin(); it != task_list.end(); it++)
    {
      auto& task = *it;
      if (not task->Done()) continue;

      task->GLSetup();
      task_list.erase(it);
      break;
    }
  }

  return Progress();
}


/////////////////////////////////////


char* Test_IMG_Load(const char* filename, unsigned ms)
{
  assert(strlen(filename) >= 1);
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  return new char(filename[0]);
}


class TestLoadTask : public Task
{
public:
  TestLoadTask(std::string filename, std::string& texture, unsigned layer)
  : filename(filename)
  , texture(texture)
  , layer(layer)
  {
    assert(layer < texture.size());
    future_surface = std::async(std::launch::async,
      Test_IMG_Load, filename.c_str(), layer * 250);
  }

private:
  std::string filename;
  std::string& texture;
  unsigned layer;

  std::future<char*> future_surface;

  bool Done() const
  {
    auto status = future_surface.wait_for(std::chrono::seconds(0));
    return status == std::future_status::ready;
  }

  void GLSetup()
  {
    assert(Done());

    auto surface = future_surface.get();

    assert(layer < texture.size());
    assert(surface);

    texture[layer] = *surface;

    delete surface;
  }
};


void test_tasks()
{

  std::string arraytexture = "...";

  TaskManager tm;

  std::cout << "Started TaskManager.  Texture string is: '" << arraytexture << "'" << std::endl;

  tm.AddTask(new TestLoadTask("1", arraytexture, 2));
  tm.AddTask(new TestLoadTask("2", arraytexture, 1));
  tm.AddTask(new TestLoadTask("3", arraytexture, 0));

  std::cout << "Added tasks.  Texture string is: '" << arraytexture << "'" << std::endl;


  while (not tm.Done())
  {
    float progress = tm.ProcessSome(100);
    std::cout << "TaskManager waiting ... " << progress << std::endl;
  }

  std::cout << "Done.  Texture string is: '" << arraytexture << "'" << std::endl;


  tm.AddTask(new TestLoadTask("1", arraytexture, 0));
  tm.AddTask(new TestLoadTask("2", arraytexture, 1));
  tm.AddTask(new TestLoadTask("3", arraytexture, 2));


  std::cout << "Added 2nd round of tasks.  Texture string is: '" << arraytexture << "'" << std::endl;

  while (not tm.Done())
  {
    float progress = tm.ProcessSome(100);
    std::cout << "TaskManager waiting ... " << progress << std::endl;
  }

  std::cout << "Done.  Texture string is: '" << arraytexture << "'" << std::endl;
}
