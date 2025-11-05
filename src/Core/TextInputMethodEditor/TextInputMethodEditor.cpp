#include "TextInputMethodEditor.hpp"
#include <SDL3/SDL_keyboard.h>

void TextInputMethodEditor::OnActivate(Rml::TextInputContext* input_context)
{
    active_input_context = input_context;
    auto window = SDL_GetKeyboardFocus();
    SDL_StartTextInput(window);
}

void TextInputMethodEditor::OnDeactivate(Rml::TextInputContext* input_context)
{
    if (active_input_context == input_context)
        active_input_context = nullptr;

    auto window = SDL_GetKeyboardFocus();
    SDL_StopTextInput(window);
}

void TextInputMethodEditor::OnDestroy(Rml::TextInputContext* input_context)
{
    if (active_input_context == input_context)
        active_input_context = nullptr;

    auto window = SDL_GetKeyboardFocus();
    SDL_StopTextInput(window);
}