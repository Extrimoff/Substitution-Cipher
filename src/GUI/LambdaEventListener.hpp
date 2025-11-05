#pragma once
#include <RmlUi/Core/EventListener.h>

class LambdaEventListener : public Rml::EventListener {
    std::function<void(Rml::Event&)> func;
public:
    explicit LambdaEventListener(std::function<void(Rml::Event&)> f)
        : func(std::move(f)) {
    }

    void ProcessEvent(Rml::Event& event) override {
        func(event);
    }
}; 