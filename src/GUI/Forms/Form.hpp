#pragma once


namespace Rml {
	class Context;
	class ElementDocument;
	class Event;
}

class Form
{
protected:
	Rml::ElementDocument*						m_doc;
	Rml::Context*								m_ctx;

public:
	Form(Rml::Context* ctx) : m_doc(nullptr), m_ctx(ctx) { }

	void Show();
};