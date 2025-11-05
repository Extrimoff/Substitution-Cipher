#include "HtmlView.hpp"

#include <RmlUi/Core.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <filesystem>

#include "../Forms/MainForm/MainForm.hpp"
#include "../Fonts/NotoEmoji-Regular.hpp"
#include "../Fonts/NotoSans-Regular.hpp"

namespace fs = std::filesystem;

static int ToRmlUiKeyModifierState(int key_modefier_state);
static Rml::Input::KeyIdentifier ToRmlUiKeyCode(int keycode);
static int ToRmlUiMouseButton(int button);

HtmlView::HtmlView(class Application* renderer, uint32_t width, uint32_t height)
    : m_renderer(renderer), m_document(nullptr) {

    m_context = Rml::CreateContext("main", Rml::Vector2i(width, height));

    Rml::LoadFontFace({ NotoMoji_Font, sizeof(NotoMoji_Font) }, "Noto Moji", Rml::Style::FontStyle::Normal, Rml::Style::FontWeight::Auto, true);
    Rml::LoadFontFace({ NotoSans_Font, sizeof(NotoSans_Font) }, "Noto Sans", Rml::Style::FontStyle::Normal);
    //std::string path = R"(D:\source\repos\SubstitutionCipher\build\x64\Debug\Fonts)";
    //for (const auto& entry : fs::directory_iterator(path)) {
    //    if (entry.is_regular_file()) {
    //        Rml::LoadFontFace(entry.path().string()) << '\n';
    //    }
    //}
    //Rml::LoadFontFace("D:\\source\\repos\\SubstitutionCipher\\build\\x64\\Debug\\NotoEmoji-Regular.ttf", 1);
}

bool HtmlView::Init()
{
  
    if (!m_context) return false;

    MainForm* form = new MainForm(m_context);
    //m_document = m_context->LoadDocument("D:\\source\\repos\\SubstitutionCipher\\build\\x64\\Debug\\hello_world.rml");
    //if (!m_document) return false;
    
    form->Show();

    return true;
}

void HtmlView::Update()
{
    m_context->Update();
}

void HtmlView::Render()
{
    m_context->Render();
}

void HtmlView::ProcessMouseMove(int x, int y, int key_modifier_state)
{
    //Rml::Log::Message(Rml::Log::LT_DEBUG, "ProcessMouseMove x: %i y: %i", x, y);
    m_context->ProcessMouseMove(x, y, ToRmlUiKeyModifierState(key_modifier_state));
}

void HtmlView::ProcessMouseButtonDown(int button_index, int key_modifier_state)
{
    //Rml::Log::Message(Rml::Log::LT_DEBUG, "ProcessMouseButtonDown button_index: %i", ToRmlUiMouseButton(button_index));
    m_context->ProcessMouseButtonDown(ToRmlUiMouseButton(button_index), ToRmlUiKeyModifierState(key_modifier_state));
}

void HtmlView::ProcessMouseButtonUp(int button_index, int key_modifier_state)
{
    //Rml::Log::Message(Rml::Log::LT_DEBUG, "ProcessMouseButtonUp button_index: %i", ToRmlUiMouseButton(button_index));
    m_context->ProcessMouseButtonUp(ToRmlUiMouseButton(button_index), ToRmlUiKeyModifierState(key_modifier_state));
}

void HtmlView::ProcessMouseLeave()
{
    m_context->ProcessMouseLeave();
}

void HtmlView::ProcessKeyUp(int key_identifier, int key_modifier_state)
{
    m_context->ProcessKeyUp(ToRmlUiKeyCode(key_identifier), ToRmlUiKeyModifierState(key_modifier_state));
}

void HtmlView::ProcessKeyDown(int key_identifier, int key_modifier_state)
{
    m_context->ProcessKeyDown(ToRmlUiKeyCode(key_identifier), ToRmlUiKeyModifierState(key_modifier_state));
}

void HtmlView::ProcessTextInput(const char* character) 
{
    m_context->ProcessTextInput(character);
}

void HtmlView::ProcessMouseWheel(float wheel_delta, int key_modifier_state)
{
    m_context->ProcessMouseWheel(wheel_delta, ToRmlUiKeyModifierState(key_modifier_state));
}

void HtmlView::SetDimensions(int x, int y)
{
    m_context->SetDimensions({x, y});
}

HtmlView::~HtmlView() {

}

static int ToRmlUiKeyModifierState(int key_modefier_state)
{
    int key_modifiers = 0;
    if (key_modefier_state & SDL_KMOD_SHIFT)    key_modifiers |= Rml::Input::KM_SHIFT;
    if (key_modefier_state & SDL_KMOD_CTRL)     key_modifiers |= Rml::Input::KM_CTRL;
    if (key_modefier_state & SDL_KMOD_ALT)      key_modifiers |= Rml::Input::KM_ALT;
    if (key_modefier_state & SDL_KMOD_GUI)      key_modifiers |= Rml::Input::KM_META;
    if (key_modefier_state & SDL_KMOD_CAPS)     key_modifiers |= Rml::Input::KM_CAPSLOCK;
    if (key_modefier_state & SDL_KMOD_NUM)      key_modifiers |= Rml::Input::KM_NUMLOCK;
    if (key_modefier_state & SDL_KMOD_SCROLL)   key_modifiers |= Rml::Input::KM_SCROLLLOCK;

    return key_modifiers;
}


static Rml::Input::KeyIdentifier ToRmlUiKeyCode(int keycode)
{
    switch (keycode)
    {
    case SDLK_UNKNOWN: return Rml::Input::KI_UNKNOWN;
    case SDLK_SPACE: return Rml::Input::KI_SPACE;
    case SDLK_RETURN: return Rml::Input::KI_RETURN;
    case SDLK_ESCAPE: return Rml::Input::KI_ESCAPE;
    case SDLK_BACKSPACE: return Rml::Input::KI_BACK;
    case SDLK_TAB: return Rml::Input::KI_TAB;
    case SDLK_CAPSLOCK: return Rml::Input::KI_CAPITAL;

    case SDLK_LSHIFT: return Rml::Input::KI_LSHIFT;
    case SDLK_RSHIFT: return Rml::Input::KI_RSHIFT;
    case SDLK_LCTRL: return Rml::Input::KI_LCONTROL;
    case SDLK_RCTRL: return Rml::Input::KI_RCONTROL;
    case SDLK_LALT: return Rml::Input::KI_LMENU;
    case SDLK_RALT: return Rml::Input::KI_RMENU;
    case SDLK_LGUI: return Rml::Input::KI_LWIN;
    case SDLK_RGUI: return Rml::Input::KI_RWIN;

    case SDLK_A: return Rml::Input::KI_A;
    case SDLK_B: return Rml::Input::KI_B;
    case SDLK_C: return Rml::Input::KI_C;
    case SDLK_D: return Rml::Input::KI_D;
    case SDLK_E: return Rml::Input::KI_E;
    case SDLK_F: return Rml::Input::KI_F;
    case SDLK_G: return Rml::Input::KI_G;
    case SDLK_H: return Rml::Input::KI_H;
    case SDLK_I: return Rml::Input::KI_I;
    case SDLK_J: return Rml::Input::KI_J;
    case SDLK_K: return Rml::Input::KI_K;
    case SDLK_L: return Rml::Input::KI_L;
    case SDLK_M: return Rml::Input::KI_M;
    case SDLK_N: return Rml::Input::KI_N;
    case SDLK_O: return Rml::Input::KI_O;
    case SDLK_P: return Rml::Input::KI_P;
    case SDLK_Q: return Rml::Input::KI_Q;
    case SDLK_R: return Rml::Input::KI_R;
    case SDLK_S: return Rml::Input::KI_S;
    case SDLK_T: return Rml::Input::KI_T;
    case SDLK_U: return Rml::Input::KI_U;
    case SDLK_V: return Rml::Input::KI_V;
    case SDLK_W: return Rml::Input::KI_W;
    case SDLK_X: return Rml::Input::KI_X;
    case SDLK_Y: return Rml::Input::KI_Y;
    case SDLK_Z: return Rml::Input::KI_Z;

    case SDLK_0: return Rml::Input::KI_0;
    case SDLK_1: return Rml::Input::KI_1;
    case SDLK_2: return Rml::Input::KI_2;
    case SDLK_3: return Rml::Input::KI_3;
    case SDLK_4: return Rml::Input::KI_4;
    case SDLK_5: return Rml::Input::KI_5;
    case SDLK_6: return Rml::Input::KI_6;
    case SDLK_7: return Rml::Input::KI_7;
    case SDLK_8: return Rml::Input::KI_8;
    case SDLK_9: return Rml::Input::KI_9;

    case SDLK_F1: return Rml::Input::KI_F1;
    case SDLK_F2: return Rml::Input::KI_F2;
    case SDLK_F3: return Rml::Input::KI_F3;
    case SDLK_F4: return Rml::Input::KI_F4;
    case SDLK_F5: return Rml::Input::KI_F5;
    case SDLK_F6: return Rml::Input::KI_F6;
    case SDLK_F7: return Rml::Input::KI_F7;
    case SDLK_F8: return Rml::Input::KI_F8;
    case SDLK_F9: return Rml::Input::KI_F9;
    case SDLK_F10: return Rml::Input::KI_F10;
    case SDLK_F11: return Rml::Input::KI_F11;
    case SDLK_F12: return Rml::Input::KI_F12;
    case SDLK_F13: return Rml::Input::KI_F13;
    case SDLK_F14: return Rml::Input::KI_F14;
    case SDLK_F15: return Rml::Input::KI_F15;
    case SDLK_F16: return Rml::Input::KI_F16;
    case SDLK_F17: return Rml::Input::KI_F17;
    case SDLK_F18: return Rml::Input::KI_F18;
    case SDLK_F19: return Rml::Input::KI_F19;
    case SDLK_F20: return Rml::Input::KI_F20;
    case SDLK_F21: return Rml::Input::KI_F21;
    case SDLK_F22: return Rml::Input::KI_F22;
    case SDLK_F23: return Rml::Input::KI_F23;
    case SDLK_F24: return Rml::Input::KI_F24;

    case SDLK_LEFT: return Rml::Input::KI_LEFT;
    case SDLK_RIGHT: return Rml::Input::KI_RIGHT;
    case SDLK_UP: return Rml::Input::KI_UP;
    case SDLK_DOWN: return Rml::Input::KI_DOWN;

    case SDLK_HOME: return Rml::Input::KI_HOME;
    case SDLK_END: return Rml::Input::KI_END;
    case SDLK_PAGEUP: return Rml::Input::KI_PRIOR;
    case SDLK_PAGEDOWN: return Rml::Input::KI_NEXT;

    case SDLK_INSERT: return Rml::Input::KI_INSERT;
    case SDLK_DELETE: return Rml::Input::KI_DELETE;

    case SDLK_PRINTSCREEN: return Rml::Input::KI_SNAPSHOT;
    case SDLK_SCROLLLOCK: return Rml::Input::KI_SCROLL;
    case SDLK_PAUSE: return Rml::Input::KI_PAUSE;

    case SDLK_KP_0: return Rml::Input::KI_NUMPAD0;
    case SDLK_KP_1: return Rml::Input::KI_NUMPAD1;
    case SDLK_KP_2: return Rml::Input::KI_NUMPAD2;
    case SDLK_KP_3: return Rml::Input::KI_NUMPAD3;
    case SDLK_KP_4: return Rml::Input::KI_NUMPAD4;
    case SDLK_KP_5: return Rml::Input::KI_NUMPAD5;
    case SDLK_KP_6: return Rml::Input::KI_NUMPAD6;
    case SDLK_KP_7: return Rml::Input::KI_NUMPAD7;
    case SDLK_KP_8: return Rml::Input::KI_NUMPAD8;
    case SDLK_KP_9: return Rml::Input::KI_NUMPAD9;
    case SDLK_KP_ENTER: return Rml::Input::KI_NUMPADENTER;
    case SDLK_KP_PLUS: return Rml::Input::KI_ADD;
    case SDLK_KP_MINUS: return Rml::Input::KI_SUBTRACT;
    case SDLK_KP_MULTIPLY: return Rml::Input::KI_MULTIPLY;
    case SDLK_KP_DIVIDE: return Rml::Input::KI_DIVIDE;
    case SDLK_KP_PERIOD: return Rml::Input::KI_DECIMAL;
    case SDLK_KP_EQUALS: return Rml::Input::KI_OEM_NEC_EQUAL;

    case SDLK_SEMICOLON: return Rml::Input::KI_OEM_1;
    case SDLK_PLUS: return Rml::Input::KI_OEM_PLUS;
    case SDLK_COMMA: return Rml::Input::KI_OEM_COMMA;
    case SDLK_MINUS: return Rml::Input::KI_OEM_MINUS;
    case SDLK_PERIOD: return Rml::Input::KI_OEM_PERIOD;
    case SDLK_SLASH: return Rml::Input::KI_OEM_2;
    case SDLK_GRAVE: return Rml::Input::KI_OEM_3;
    case SDLK_LEFTBRACKET: return Rml::Input::KI_OEM_4;
    case SDLK_BACKSLASH: return Rml::Input::KI_OEM_5;
    case SDLK_RIGHTBRACKET: return Rml::Input::KI_OEM_6;
    case SDLK_APOSTROPHE: return Rml::Input::KI_OEM_7;
    case SDLK_LESS: return Rml::Input::KI_OEM_102;

    case SDLK_VOLUMEUP: return Rml::Input::KI_VOLUME_UP;
    case SDLK_VOLUMEDOWN: return Rml::Input::KI_VOLUME_DOWN;
    case SDLK_MUTE: return Rml::Input::KI_VOLUME_MUTE;
    case SDLK_MEDIA_NEXT_TRACK: return Rml::Input::KI_MEDIA_NEXT_TRACK;
    case SDLK_MEDIA_PREVIOUS_TRACK: return Rml::Input::KI_MEDIA_PREV_TRACK;
    case SDLK_MEDIA_STOP: return Rml::Input::KI_MEDIA_STOP;
    case SDLK_MEDIA_PLAY_PAUSE: return Rml::Input::KI_MEDIA_PLAY_PAUSE;
    case SDLK_POWER: return Rml::Input::KI_POWER;
    case SDLK_SLEEP: return Rml::Input::KI_SLEEP;
    case SDLK_WAKE: return Rml::Input::KI_WAKE;
    case SDLK_APPLICATION: return Rml::Input::KI_APPS;
    case SDLK_MENU: return Rml::Input::KI_APPS;

    case SDLK_LMETA: return Rml::Input::KI_LMETA;
    case SDLK_RMETA: return Rml::Input::KI_RMETA;

    default: return Rml::Input::KI_UNKNOWN;
    }
}

static int ToRmlUiMouseButton(int button) {
    switch (button)
    {
    case SDL_BUTTON_LEFT: return 0;
    case SDL_BUTTON_RIGHT: return 1;
    case SDL_BUTTON_MIDDLE: return 2;
    default: return 3;
    }
}