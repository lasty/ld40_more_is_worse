
#include <ctime>
#include <iostream>
#include <sstream>
#include <stdexcept>


constexpr int SWAP_INTERVAL{1};

constexpr int GL_MAJOR{3};
constexpr int GL_MINOR{3};

constexpr int WIDTH = 1280;
constexpr int HEIGHT = 768;


#include <SDL.h>
#undef main


#include "gl.hpp"

#include "game.hpp"
#include "maths.hpp"
#include "renderer.hpp"
#include "sound.hpp"
#include "to_string.hpp"


void ProcessEvents(Game *game, [[maybe_unused]] Renderer *renderer)
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
      case SDL_QUIT:
        game->gamestate.running = false;
        break;

      case SDL_KEYDOWN:
        game->ProcessKeyInput(event.key.keysym.sym, true);
        break;
      case SDL_KEYUP:
        game->ProcessKeyInput(event.key.keysym.sym, false);
        break;

      case SDL_MOUSEBUTTONDOWN:
        game->ProcessMouseInput(event.button.button, true);
        break;

      case SDL_MOUSEBUTTONUP:
        game->ProcessMouseInput(event.button.button, false);
        break;

      case SDL_MOUSEMOTION:
        game->ProcessMouseMotion(event.motion.x, event.motion.y);
        break;
    }
  }
}


class Timer
{
public:
  uint32_t time_start = SDL_GetTicks();
  uint32_t Report() const { return SDL_GetTicks() - time_start; };
};

std::ostream &operator<<(std::ostream &out, const Timer &timer)
{
  return out << timer.Report() << "ms";
}

void main_game()
{
  std::cout << "Hello, world" << std::endl;
  std::cout.precision(2);
  std::cout << std::fixed;

  SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);

  SDL_version linked;
  SDL_version compiled;

  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);

  std::cout << "SDL Version: "
            << "(compiled with " << (int)compiled.major << "." << (int)compiled.minor << "." << (int)compiled.patch << ")"
            << "  (linked with " << (int)linked.major << "." << (int)linked.minor << "." << (int)linked.patch << ")"
            << std::endl;


  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_MAJOR);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_MINOR);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  std::cout << "Requesting OpenGL Context version " << GL_MAJOR << "." << GL_MINOR
            << std::endl;

  Timer timer_window;

  unsigned int window_flags = SDL_WINDOW_OPENGL;
  SDL_Window *window = SDL_CreateWindow("ld40", 50, 50, WIDTH, HEIGHT, window_flags);

  if (not window)
  {
    throw std::runtime_error("failed to create window");
  }

  std::cout << "Created window in " << timer_window << std::endl;

  auto glcontext = SDL_GL_CreateContext(window);

  //load opengl extension library here
  if (glewInit() != GLEW_OK)
  {
    throw std::runtime_error("failed to init GLEW");
  }

  std::cout << "GLEW Version: " << glewGetString(GLEW_VERSION) << std::endl;
  std::cout << "GL Version String: " << glGetString(GL_VERSION) << std::endl;

  int gl_version_maj = GL::GetInteger(GL_MAJOR_VERSION);
  int gl_version_min = GL::GetInteger(GL_MINOR_VERSION);


  std::cout << "GL Version: " << gl_version_maj << "." << gl_version_min << std::endl;

  int profile = 0;
  SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);

  std::cout << "GL Profile: ";
  if (profile == SDL_GL_CONTEXT_PROFILE_COMPATIBILITY)
    std::cout << "COMPATABILITY";
  else if (profile == SDL_GL_CONTEXT_PROFILE_CORE)
    std::cout << "CORE";
  else if (profile == SDL_GL_CONTEXT_PROFILE_ES)
    std::cout << "ES";
  else
    std::cout << "(Unknown/Default?)";
  std::cout << std::endl;


  GL::Debuging(true);

  int max_array_texture_layers = GL::GetInteger(GL_MAX_ARRAY_TEXTURE_LAYERS);
  std::cout << "Max Array Texture Layers: " << max_array_texture_layers << std::endl;


  SDL_GL_SetSwapInterval(SWAP_INTERVAL);

  {
    Timer timer_game_start;
    Game game;
    game.NewGame();
    std::cout << "Game state created in " << timer_game_start << std::endl;


    Timer timer_renderer_start;
    Renderer renderer;
    renderer.Resize(WIDTH, HEIGHT);
    std::cout << "Renderer created in " << timer_renderer_start << std::endl;


#if !NDEBUG
    //If in Debug mode, set extra state things here
#endif

    auto last_time = SDL_GetTicks();

    // Main Loop
    while (game.gamestate.running)
    {

      ProcessEvents(&game, &renderer);

      auto this_time = SDL_GetTicks();
      float delta_time = (this_time - last_time) / 1000.0f;
      last_time = this_time;

      game.RemoveDeadItems();
      game.Update(delta_time);

      // Render
      renderer.RenderAll(game);
      SDL_GL_SwapWindow(window);

    } // end main loop
  }
  //Clean up

  SDL_GL_DeleteContext(glcontext);

  SDL_DestroyWindow(window);

  SDL_Quit();

  std::cout << "Game exited normally" << std::endl;
}


constexpr bool RUN_TESTS = false;

void test_utf8();

void run_tests()
{
  std::cout << "CPP version: " << CPPVersion() << std::endl;

  test_utf8();
}

constexpr bool CATCH_EXCEPTIONS = true;


int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
  if constexpr (RUN_TESTS)
  {
    run_tests();
    return EXIT_SUCCESS;
  }

  std::cout << "CPP version: " << CPPVersion() << std::endl;

  if constexpr (CATCH_EXCEPTIONS)
  {
    try
    {
      main_game();
    }
    catch (std::exception &e)
    {
      std::cout << "std::exception thrown -- " << e.what() << std::endl;
    }
  }
  else
  {

    main_game();
  }

  return EXIT_SUCCESS;
}
