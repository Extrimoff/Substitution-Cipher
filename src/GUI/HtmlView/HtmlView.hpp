#pragma once
#include <cstdint>

namespace Rml {
	class Context;
	class ElementDocument;
}

class HtmlView
{
private:
	class Application*				m_renderer;
	Rml::Context*					m_context;
	Rml::ElementDocument*			m_document;
public:
	HtmlView(class Application* renderer, uint32_t width, uint32_t height);
	~HtmlView();

	bool Init();

	void Update();
	void Render();
	void ProcessMouseMove(int x, int y, int key_modifier_state = 0);
	void ProcessMouseButtonDown(int button_index, int key_modifier_state = 0);
	void ProcessMouseButtonUp(int button_index, int key_modifier_state = 0);
	void ProcessMouseLeave();
	void ProcessKeyUp(int key_identifier, int key_modifier_state);
	void ProcessKeyDown(int key_identifier, int key_modifier_state);
	void ProcessTextInput(const char* character);
	void ProcessMouseWheel(float delta, int key_modifier_state = 0);
	void SetDimensions(int x, int y);
};