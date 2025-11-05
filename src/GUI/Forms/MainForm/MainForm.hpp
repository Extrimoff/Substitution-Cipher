#include "../Form.hpp"
#include <string>
#include <map>

class MainForm : public Form
{
	enum class Alphabet {
		RUS,
		MIXED
	};

private:
	std::map<Alphabet, std::string> m_alphabetList;
	std::string						m_keyword;
	std::string						m_separator;
	std::string						m_sourceText;
	std::string						m_result;
	Alphabet						m_alphabet;

	void EncryptText(Rml::Event& event);
	void DecryptText(Rml::Event& event);
	void CopyText(Rml::Event& event);
	void SaveTextToFile(Rml::Event& event);
	void LoadTextFromFile(Rml::Event& event);
	void ChangeAlphabet(Rml::Event& event);
	void ChangeSeparator(Rml::Event& event);

public:
	MainForm(Rml::Context* ctx);
};