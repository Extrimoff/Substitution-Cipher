#pragma once
#include <RmlUi/Core/SystemInterface.h>
#include <RmlUi/Core/TextInputHandler.h>

class TextInputMethodEditor : public Rml::TextInputHandler {
public:
    void OnActivate(Rml::TextInputContext* input_context) override;
    void OnDeactivate(Rml::TextInputContext* input_context) override;
    void OnDestroy(Rml::TextInputContext* input_context) override;

private:
    Rml::TextInputContext* active_input_context = nullptr;
};