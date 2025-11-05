#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <locale>
#include "Core/Application/Application.hpp"

#include "Core/Cipher/Cipher.hpp"

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    auto renderer = new Application(900, 700);
    *appstate = renderer;
    return renderer->AppInit(argc, argv);
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    auto container = reinterpret_cast<Application*>(appstate);
    return container->AppEvent(event);
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    auto container = reinterpret_cast<Application*>(appstate);
    return container->AppIterate();
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    auto container = reinterpret_cast<Application*>(appstate);
    return container->AppQuit(result);
}