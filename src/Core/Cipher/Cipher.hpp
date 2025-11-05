#pragma once
#include <string>
#include <vector>

class Cipher 
{
	struct Table {
		std::vector<std::vector<char32_t>> table;
		std::vector<std::u32string> rowKeys;
		std::vector<std::u32string> colKeys;
	};

private:
	std::u32string m_alphabet;
	std::u32string m_separator;

	Table BuildTable(std::u32string const& key);
	std::u32string GetUniqueKey(std::u32string const& key);
public:
	Cipher(std::string const& alphabet, std::string const& separator = " ");

	std::string Encode(std::string const& text, std::string const& keyword); 
	std::string Decode(std::string const& text, std::string const& keyword);
public:
	std::string GetAlphabet() const;
	void SetAlphabet(std::string const& alphabet);
	__declspec(property(get = GetAlphabet, put = SetAlphabet)) std::string alphabet;

	std::string GetSeparator() const;
	void SetSeparator(std::string const& separator);
	__declspec(property(get = GetSeparator, put = SetSeparator)) std::string separator;
};