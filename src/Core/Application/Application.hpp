#pragma once
#include <SDL3/SDL.h>
#include <RmlUi/Core/RenderInterface.h>

class Application : public Rml::RenderInterface, public std::enable_shared_from_this<Application>
{
    struct GeometryData {
        std::vector<Rml::Vertex> vertices;
        std::vector<int> indices;
    };

    using textures_t = std::unordered_map<Rml::TextureHandle, struct SDL_Texture*>;
    using geometry_t = std::unordered_map<Rml::CompiledGeometryHandle, struct GeometryData>;

private:
    std::unique_ptr<class HtmlView>                 m_view;
    std::unique_ptr<class TextInputMethodEditor>    m_text_input_method_editor;
    std::unique_ptr<class SystemInterface>          m_system_interface;
    std::unique_ptr<class FileInterface>            m_file_interface;

    struct SDL_Renderer*                            m_renderer;
    struct SDL_Window*                              m_window;
    uint32_t                                        m_height;
    uint32_t                                        m_width;

    Rml::TextureHandle					            m_next_texture;
    Rml::CompiledGeometryHandle			            m_next_geometry;
    textures_t							            m_textures;
    geometry_t					                    m_geometry;
    std::unique_ptr<struct SDL_Rect>	            m_scissor_rect;
    bool								            m_scissor_enabled;



public:
    Application(uint32_t width, uint32_t height);
    ~Application();

    Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override;
    void RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture) override;
    void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;
    Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
    Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override;
    void ReleaseTexture(Rml::TextureHandle texture) override;
    void EnableScissorRegion(bool enable) override;
    void SetScissorRegion(Rml::Rectanglei region) override;

    void EnableClipMask(bool enable) override {}
    void RenderToClipMask(Rml::ClipMaskOperation, Rml::CompiledGeometryHandle, Rml::Vector2f) override {}
    void SetTransform(const Rml::Matrix4f*) override {}
    Rml::LayerHandle PushLayer() override { return 0; }
    void CompositeLayers(Rml::LayerHandle, Rml::LayerHandle, Rml::BlendMode, Rml::Span<const Rml::CompiledFilterHandle>) override {}
    void PopLayer() override {}
    Rml::TextureHandle SaveLayerAsTexture() override { return 0; }
    Rml::CompiledFilterHandle SaveLayerAsMaskImage() override { return 0; }
    Rml::CompiledFilterHandle CompileFilter(const Rml::String&, const Rml::Dictionary&) override { return 0; }
    void ReleaseFilter(Rml::CompiledFilterHandle) override {}
    Rml::CompiledShaderHandle CompileShader(const Rml::String&, const Rml::Dictionary&) override { return 0; }
    void RenderShader(Rml::CompiledShaderHandle, Rml::CompiledGeometryHandle, Rml::Vector2f, Rml::TextureHandle) override {}
    void ReleaseShader(Rml::CompiledShaderHandle) override {}

    SDL_AppResult AppInit(int argc, char** argv);
    SDL_AppResult AppIterate();
    SDL_AppResult AppEvent(SDL_Event* event);
    void AppQuit(SDL_AppResult result);

    void render();

private:
    void size_changed();
};