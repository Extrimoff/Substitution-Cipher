#include "Form.hpp"

#include <RmlUi/Core.h>
#include <exception>

void Form::Show()
{
	if (!m_doc) throw std::exception("Document was not initialized");
	m_doc->Show();
}