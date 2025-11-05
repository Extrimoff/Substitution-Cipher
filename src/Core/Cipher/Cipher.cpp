#include "Cipher.hpp"
#include "../../Utils/StringUtils.hpp"

#include <random>
#include <algorithm>
#include <unordered_map>
#include <span>
#include <ranges>



Cipher::Cipher(std::string const& alphabet, std::string const& separator)
{
	m_alphabet = string_utils::utf8_to_u32(alphabet);
	m_separator = string_utils::utf8_to_u32(separator);
}

Cipher::Table Cipher::BuildTable(std::u32string const& key) 
{
	std::u32string base = key;
	for (auto c : m_alphabet)
		if (base.find(c) == std::u32string::npos)
			base += c;

	int total = static_cast<int>(base.size());
	int cols = static_cast<int>(ceil(sqrt(total)));
	int rows = static_cast<int>(ceil(static_cast<double>(total) / cols));

	Cipher::Table table{
		std::vector<std::vector<char32_t>>(rows, std::vector<char32_t>(cols, U'\0')),
		std::vector<std::u32string>(rows),
		std::vector<std::u32string>(cols)
	};

	int k = 0;
	for (int i = 0; i < rows && k < total; i++)
		for (int j = 0; j < cols && k < total; j++)
			table.table[i][j] = base[k++];

	auto generateTableKeys = [&](const std::u32string& key, int count,
                             std::span<const char32_t> symbols) -> std::vector<std::u32string>
	{
		uint64_t seed = string_utils::str_hash(key);
		std::mt19937_64 rng(seed);

		std::vector<char32_t> local_symbols(symbols.begin(), symbols.end());
		std::shuffle(local_symbols.begin(), local_symbols.end(), rng);

		std::vector<std::u32string> res;
		res.reserve(count);

		size_t idx = 0;
		for (int i = 0; i < count; ++i) {
			char32_t a = local_symbols[idx++];
			char32_t b = local_symbols[idx++];
			res.emplace_back(std::u32string{a, b});

			if (idx + 1 >= local_symbols.size()) {
				std::shuffle(local_symbols.begin(), local_symbols.end(), rng);
				idx = 0;
			}
		}

		return res;
	};
	uint64_t seed = string_utils::str_hash(key);
	std::mt19937_64 rng(seed);
	std::vector<char32_t> symbols(m_alphabet.begin(), m_alphabet.end());
	std::shuffle(symbols.begin(), symbols.end(), rng);
	size_t half = symbols.size() / 2;

	std::span<const char32_t> row_symbols(symbols.data(), half);
	std::span<const char32_t> col_symbols(symbols.data() + half, symbols.size() - half);

	table.rowKeys = generateTableKeys(key, rows, row_symbols);
	table.colKeys = generateTableKeys(key + U"_col", cols, col_symbols);


	return table;
}

std::u32string Cipher::GetUniqueKey(std::u32string const& key)
{
	std::u32string res;
	for (auto c : key) {
		if (res.find(c) == std::u32string::npos && m_alphabet.find(c) != std::u32string::npos) {
			res += c;
		}
	}
	return res;
}

std::string Cipher::Encode(std::string const& oText, std::string const& oKeyword)
{
	std::u32string key = string_utils::utf8_to_u32(oKeyword);
	std::u32string text = string_utils::utf8_to_u32(oText);

	auto table = BuildTable(GetUniqueKey(key));

	std::u32string result;
	std::unordered_map<char32_t, int> counters;
	for (auto c : text) {
		if (m_alphabet.find(c) == std::u32string::npos) {
			result += c;
			continue;
		}

		std::vector<std::u32string> combos;

		for (size_t i = 0; i < table.table.size(); ++i) {
			for (size_t j = 0; j < table.table[i].size(); ++j) {
				if (table.table[i][j] == c) {
					const auto& r = table.rowKeys[i];
					const auto& col = table.colKeys[j];

					for (auto rc : r)
						for (auto cc : col) {
							combos.push_back({ rc, cc });
							combos.push_back({ cc, rc });
						}
				}
			}
		}

		uint64_t seed = string_utils::str_hash(key + std::u32string(1, c));
		std::mt19937_64 rng(seed);
		std::shuffle(combos.begin(), combos.end(), rng);

		int index = counters[c] % combos.size();
		std::u32string chosen = combos[index];
		counters[c]++;
		for (auto ch : chosen) result += ch;
		result += m_separator;
	}

	return string_utils::u32_to_utf8(result);
}

std::string Cipher::Decode(std::string const& oText, std::string const& oKeyword)
{
	std::u32string key = string_utils::utf8_to_u32(oKeyword);
	std::u32string text = string_utils::utf8_to_u32(oText);

	auto table = BuildTable(GetUniqueKey(key));

	std::u32string result;
	for (size_t i = 0; i + 1 < text.size(); i += 2 + m_separator.length()) {
		while (m_alphabet.find(text[i]) == std::u32string::npos && i + 1 < text.size()) {
			result += text[i];
			i++;
		}
		std::u32string token;
		token = text[i];
		token += text[i + 1];

		bool found = false;
		for (int j = 0; j < table.table.size() && !found; j++) {
			for (int k = 0; k < table.table[j].size() && !found; k++) {
				const std::u32string& r = table.rowKeys[j];
				const std::u32string& col = table.colKeys[k];

				std::vector<std::u32string> combos;
				for (char32_t rc : r) {
					for (char32_t cc : col) {
						combos.push_back({ rc, cc });
						combos.push_back({ cc, rc });
					}
				}

				for (auto const& candidate : combos) {
					if (candidate == token) {
						result.push_back(table.table[j][k]);
						found = true;
						break;
					}
				}
			}
		}
	}

	return string_utils::u32_to_utf8(result);
}

std::string Cipher::GetAlphabet() const
{
	return string_utils::u32_to_utf8(m_alphabet);
}

void Cipher::SetAlphabet(std::string const& alphabet)
{
	m_alphabet = string_utils::utf8_to_u32(alphabet);
}


std::string Cipher::GetSeparator() const
{
	return string_utils::u32_to_utf8(m_separator);
}

void Cipher::SetSeparator(std::string const& separator)
{
	m_separator = string_utils::utf8_to_u32(separator);
}
