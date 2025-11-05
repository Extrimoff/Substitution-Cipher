#include "Application.hpp"
#include "../../GUI/HtmlView/HtmlView.hpp"
#include "../TextInputMethodEditor/TextInputMethodEditor.hpp"
#include "../SystemInterface/SystemInterface.hpp"
#include "../FileInterface/FileInterface.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <RmlUi/Core.h>


Application::Application(uint32_t width, uint32_t height)
    : m_view(nullptr), m_renderer(nullptr), m_window(nullptr), m_width(width), m_height(height),
    m_next_texture(1), m_next_geometry(1), m_textures(textures_t()),
    m_geometry(geometry_t()), m_scissor_rect(std::make_unique<SDL_Rect>()), m_scissor_enabled(false) {

}

Application::~Application() {
    for (auto& pair : m_textures)
        SDL_DestroyTexture(pair.second);
}


SDL_AppResult Application::AppInit(int argc, char** argv)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        Rml::Log::Message(Rml::Log::LT_ERROR, "Unable to initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_CreateWindowAndRenderer("Вариантный шифр", m_width, m_height, SDL_WINDOW_RESIZABLE, &m_window, &m_renderer);

    if (!m_window) {
        Rml::Log::Message(Rml::Log::LT_ERROR, "Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return SDL_APP_FAILURE;
    }

    if (!m_renderer) {
        Rml::Log::Message(Rml::Log::LT_ERROR, "Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return SDL_APP_FAILURE;
    }

    Rml::SetRenderInterface(this);

    m_text_input_method_editor = std::make_unique<TextInputMethodEditor>();
    Rml::SetTextInputHandler(m_text_input_method_editor.get());

    m_system_interface = std::make_unique<SystemInterface>();
    Rml::SetSystemInterface(m_system_interface.get());

    m_file_interface = std::make_unique<FileInterface>();
    Rml::SetFileInterface(m_file_interface.get());

    Rml::Initialise();

    m_view = std::make_unique<HtmlView>(this, m_width, m_height);

    if (!m_view->Init()) return SDL_APP_FAILURE;

    return SDL_APP_CONTINUE;
}

SDL_AppResult Application::AppIterate()
{
    m_view->Update();

    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    m_view->Render();

    SDL_RenderPresent(m_renderer);
    return SDL_APP_CONTINUE;
}

SDL_AppResult Application::AppEvent(SDL_Event* event)
{
    switch (event->type) {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
        break;

    case SDL_EVENT_WINDOW_RESIZED:
        m_view->SetDimensions(event->window.data1, event->window.data2);
        break;
    
    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        m_view->ProcessMouseLeave();
        break;

    case SDL_EVENT_KEY_UP:
        m_view->ProcessKeyUp(event->key.key, event->key.mod);
        break;
    
    case SDL_EVENT_KEY_DOWN:
        m_view->ProcessKeyDown(event->key.key, event->key.mod);
        break;
    
    case SDL_EVENT_TEXT_INPUT:
        m_view->ProcessTextInput(event->text.text);
        break;
    
    case SDL_EVENT_MOUSE_WHEEL:
        m_view->ProcessMouseWheel(-event->wheel.y, SDL_GetModState());
        break;
    
    case SDL_EVENT_MOUSE_MOTION:
    {
        const float pixel_density = SDL_GetWindowPixelDensity(m_window);
        m_view->ProcessMouseMove(static_cast<int>(event->motion.x * pixel_density), static_cast<int>(event->motion.y * pixel_density), SDL_GetModState());
        break;
    }

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        m_view->ProcessMouseButtonDown(event->button.button, SDL_GetModState());
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        m_view->ProcessMouseButtonUp(event->button.button, SDL_GetModState());
        break;

    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

void Application::AppQuit(SDL_AppResult result)
{
    Rml::Shutdown();

    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

// ----------------- Geometry -----------------
Rml::CompiledGeometryHandle Application::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) {
    auto handle = m_next_geometry++;
    GeometryData geometry_data;
    geometry_data.vertices.assign(vertices.begin(), vertices.end());
    geometry_data.indices.assign(indices.begin(), indices.end());
    m_geometry[handle] = geometry_data;
    return handle;
}

void Application::RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture) {
    if (m_geometry.find(geometry) == m_geometry.end())
        return;

    const auto& geometry_data = m_geometry[geometry];
    const auto& vertices = geometry_data.vertices;
    const auto& indices = geometry_data.indices;

    SDL_Texture* sdl_texture = nullptr;
    if (texture != 0) {
        auto it = m_textures.find(texture);
        if (it != m_textures.end()) {
            sdl_texture = it->second;
        }
    }

    // Создаем массив вертексов для SDL
    std::vector<SDL_Vertex> sdl_vertices;
    sdl_vertices.reserve(vertices.size());

    // Конвертируем вершины RmlUi в вершины SDL
    for (const auto& vertex : vertices) {
        SDL_Vertex sdl_vertex;
        sdl_vertex.position.x = vertex.position.x + translation.x;
        sdl_vertex.position.y = vertex.position.y + translation.y;

        // Цвет
        sdl_vertex.color.r = vertex.colour.red / 255.0f;
        sdl_vertex.color.g = vertex.colour.green / 255.0f;
        sdl_vertex.color.b = vertex.colour.blue / 255.0f;
        sdl_vertex.color.a = vertex.colour.alpha / 255.0f;

        // Текстурные координаты
        sdl_vertex.tex_coord.x = vertex.tex_coord.x;
        sdl_vertex.tex_coord.y = vertex.tex_coord.y;

        sdl_vertices.push_back(sdl_vertex);
    }

    // Конвертируем индексы
    std::vector<int> sdl_indices(indices.begin(), indices.end());

    // Рендерим геометрию
    if (sdl_texture) {
        // Для текстурированной геометрии (текст)
        SDL_RenderGeometry(m_renderer, sdl_texture,
            sdl_vertices.data(), static_cast<int>(sdl_vertices.size()),
            sdl_indices.data(), static_cast<int>(sdl_indices.size()));
    }
    else {
        // Для нетекстурированной геометрии
        SDL_RenderGeometry(m_renderer, nullptr,
            sdl_vertices.data(), static_cast<int>(sdl_vertices.size()),
            sdl_indices.data(), static_cast<int>(sdl_indices.size()));
    }
}

void Application::ReleaseGeometry(Rml::CompiledGeometryHandle geometry) {
    m_geometry.erase(geometry);
}

// ----------------- Texture -----------------
Rml::TextureHandle Application::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) {

    SDL_Surface* surface = IMG_Load_IO(reinterpret_cast<SDL_IOStream*>(const_cast<char*>(source.c_str())), true);
    if (!surface) {
        Rml::Log::Message(Rml::Log::LT_ERROR, "Failed to load image: %s, error: %s", source.c_str(), SDL_GetError());
        return 0;
    }

    // Конвертируем в RGBA32 если необходимо
    SDL_Surface* converted_surface = nullptr;
    if (surface->format != SDL_PIXELFORMAT_RGBA32) {
        converted_surface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(surface);
        if (!converted_surface) {
            Rml::Log::Message(Rml::Log::LT_ERROR, "Failed to convert surface format");
            return 0;
        }
        surface = converted_surface;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_DestroySurface(surface);

    if (!texture) {
        Rml::Log::Message(Rml::Log::LT_ERROR, "Failed to create texture: %s", SDL_GetError());
        return 0;
    }

    // Устанавливаем blending для текстуры (важно для текста)
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    Rml::TextureHandle handle = m_next_texture++;
    m_textures[handle] = texture;

    // Получаем размер текстуры
    float w, h;
    SDL_GetTextureSize(texture, &w, &h);
    texture_dimensions.x = static_cast<int>(w);
    texture_dimensions.y = static_cast<int>(h);

    Rml::Log::Message(Rml::Log::LT_DEBUG, "Loaded texture: %s, size: %dx%d, handle: %d",
        source.c_str(), w, h, handle);

    return handle;
}

Rml::TextureHandle Application::GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) {
    // Эта функция используется для генерации текстур шрифтов
    if (source.empty() || source_dimensions.x == 0 || source_dimensions.y == 0) {
        return 0;
    }

    // Создаем поверхность для текстуры
    SDL_Surface* surface = SDL_CreateSurface(source_dimensions.x, source_dimensions.y, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        Rml::Log::Message(Rml::Log::LT_ERROR, "Failed to create surface for generated texture");
        return 0;
    }

    // Копируем данные (предполагаем RGBA формат)
    if (source.size() == (size_t)source_dimensions.x * source_dimensions.y * 4) {
        memcpy(surface->pixels, source.data(), source.size());
    }
    else {
        // Если данные в другом формате, конвертируем
        Rml::Log::Message(Rml::Log::LT_WARNING, "Warning: Texture data size doesn't match expected size");
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_DestroySurface(surface);

    if (!texture) {
        Rml::Log::Message(Rml::Log::LT_ERROR, "Failed to create texture from generated surface");
        return 0;
    }

    // Устанавливаем blending для текстуры
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    Rml::TextureHandle handle = m_next_texture++;
    m_textures[handle] = texture;

    Rml::Log::Message(Rml::Log::LT_DEBUG, "Generated texture, size: %dx%d, handle: %d",
        source_dimensions.x, source_dimensions.y, handle);

    return handle;
}

void Application::ReleaseTexture(Rml::TextureHandle texture) {
    auto it = m_textures.find(texture);
    if (it != m_textures.end()) {
        SDL_DestroyTexture(it->second);
        m_textures.erase(it);
        Rml::Log::Message(Rml::Log::LT_DEBUG, "Released texture: %d", texture);
    }
}

// ----------------- Scissor -----------------
void Application::EnableScissorRegion(bool enable) {
    m_scissor_enabled = enable;
    if (!enable) {
        SDL_SetRenderClipRect(m_renderer, nullptr);
    }
    else if (m_scissor_rect->w > 0 && m_scissor_rect->h > 0) {
        SDL_SetRenderClipRect(m_renderer, m_scissor_rect.get());
    }
}

void Application::SetScissorRegion(Rml::Rectanglei region) {
    m_scissor_rect->x = region.Left();
    m_scissor_rect->y = region.Top();
    m_scissor_rect->w = region.Width();
    m_scissor_rect->h = region.Height();

    if (m_scissor_enabled) {
        SDL_SetRenderClipRect(m_renderer, m_scissor_rect.get());
    }
}