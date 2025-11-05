#include "SystemInterface.hpp"

#include <SDL3/SDL.h>
#include <filesystem>

double SystemInterface::GetElapsedTime() {
    static const Uint64 start = SDL_GetPerformanceCounter();
    static const double frequency = static_cast<double>(SDL_GetPerformanceFrequency());
    return static_cast<double>((SDL_GetPerformanceCounter() - m_start_time) / m_frequency);
}

SystemInterface::SystemInterface() : m_start_time(SDL_GetPerformanceCounter()), 
m_frequency(static_cast<double>(SDL_GetPerformanceFrequency())), m_current_cursor(nullptr) {

}

bool SystemInterface::LogMessage(Rml::Log::Type type, const Rml::String& message) 
{
	const char* prefix;
    bool doLog = true;
	switch (type)
	{
	case Rml::Log::LT_ERROR:
	case Rml::Log::LT_ASSERT: prefix = "-!-"; break;

	case Rml::Log::LT_WARNING: prefix = "-*-"; break;

#ifndef _DEBUG
    default: doLog = false; prefix = "---"; break;
#else
    default: prefix = "---"; break;
#endif 

	}
    if(doLog) SDL_Log("%s (%.4f): %s\n", prefix, GetElapsedTime(), message.c_str());
    return true;
}

void SystemInterface::SetMouseCursor(const Rml::String& cursor_name) 
{
    static std::unordered_map<std::string, SDL_SystemCursor> cursor_map = {
        {"arrow", SDL_SYSTEM_CURSOR_DEFAULT},
        {"text", SDL_SYSTEM_CURSOR_TEXT},
        {"pointer", SDL_SYSTEM_CURSOR_POINTER},
        {"wait", SDL_SYSTEM_CURSOR_WAIT},
        {"cross", SDL_SYSTEM_CURSOR_CROSSHAIR},
        {"resize", SDL_SYSTEM_CURSOR_MOVE},
        {"resize_nesw", SDL_SYSTEM_CURSOR_NESW_RESIZE},
        {"resize_nwse", SDL_SYSTEM_CURSOR_NWSE_RESIZE},
        {"resize_ns", SDL_SYSTEM_CURSOR_NS_RESIZE},
        {"resize_ew", SDL_SYSTEM_CURSOR_EW_RESIZE},
        {"not-allowed", SDL_SYSTEM_CURSOR_NOT_ALLOWED}
    };

    auto it = cursor_map.find(cursor_name);
    SDL_SystemCursor sdl_cursor = SDL_SYSTEM_CURSOR_DEFAULT;

    if (it != cursor_map.end())
        sdl_cursor = it->second;

    if (m_current_cursor) {
        SDL_DestroyCursor(m_current_cursor);
        m_current_cursor = nullptr;
    }

    m_current_cursor = SDL_CreateSystemCursor(sdl_cursor);
    SDL_SetCursor(m_current_cursor);
}

void SystemInterface::SetClipboardText(const Rml::String& text) 
{
	SDL_SetClipboardText(text.c_str());
}

//void SystemInterface::ActivateKeyboard(Rml::Vector2f caret_position, float line_height)
//{
//    auto window = SDL_GetKeyboardFocus();
//    const SDL_Rect rect = { int(caret_position.x), int(caret_position.y), 1, int(line_height) };
//    SDL_SetTextInputArea(window, &rect, 0);
//    SDL_StartTextInput(window);
//}
//
//void SystemInterface::DeactivateKeyboard()
//{
//    auto window = SDL_GetKeyboardFocus();
//    SDL_StopTextInput(window);
//}

void SystemInterface::GetClipboardText(Rml::String& text) 
{
    char* raw_text = SDL_GetClipboardText();
    text = Rml::String(raw_text);
    SDL_free(raw_text);
}