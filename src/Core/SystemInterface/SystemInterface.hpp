#pragma once
#include <RmlUi/Core/SystemInterface.h>

class SystemInterface : public Rml::SystemInterface {
private:
	struct SDL_Cursor* m_current_cursor;
	const uint64_t m_start_time;
	const double m_frequency;
public:
	SystemInterface();

	~SystemInterface() override {

	}

	double GetElapsedTime() override;
	bool LogMessage(Rml::Log::Type type, const Rml::String& message) override;
	void SetMouseCursor(const Rml::String& cursor_name) override;
	void SetClipboardText(const Rml::String& text) override;
	void GetClipboardText(Rml::String& text) override;
};