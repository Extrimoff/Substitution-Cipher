#include "MainForm.hpp"
#include "../../LambdaEventListener.hpp"
#include "../../../Core/Cipher/Cipher.hpp"
#include "../../../Core/FileInterface/FileInterface.hpp"
#include "../../../Utils/StringUtils.hpp"

#include <tinyfiledialogs/tinyfiledialogs.h>
#include <RmlUi/Core.h>
#include <map>

static std::unique_ptr<Cipher> cip = nullptr;

void MainForm::EncryptText(Rml::Event& event) 
{
	if (m_alphabet == Alphabet::RUS) {
		std::string newText = string_utils::to_upper(m_sourceText);
		std::string newKeyword = string_utils::to_upper(m_keyword);
		m_result = cip->Encode(newText, newKeyword);
	}
	else {
		m_result = cip->Encode(m_sourceText, m_keyword);
	}
	auto model = m_ctx->GetDataModel("form_model");
	model.GetModelHandle().DirtyVariable("result");
}

void MainForm::DecryptText(Rml::Event& event) 
{
	if (m_alphabet == Alphabet::RUS) {
		std::string newKeyword = string_utils::to_upper(m_keyword);
		m_result = cip->Encode(m_sourceText, newKeyword);
	}
	else {
		m_result = cip->Encode(m_sourceText, m_keyword);
	}
	m_result = cip->Decode(m_sourceText, m_keyword);
	auto model = m_ctx->GetDataModel("form_model");
	model.GetModelHandle().DirtyVariable("result");
}

void MainForm::CopyText(Rml::Event& event) 
{
	Rml::GetSystemInterface()->SetClipboardText(m_result);
}

void MainForm::SaveTextToFile(Rml::Event& event) 
{
	const char* filters[] = { "*.txt" };
	const char* file = tinyfd_saveFileDialog(
		"Сохранить файл как", 
		"output.txt",         
		1,                    
		filters,              
		"Текстовые файлы"    
	);

	if (!file) return;
	
	FileInterface::WriteToFile(file, m_result);
}

void MainForm::LoadTextFromFile(Rml::Event& event) 
{
	const char* filters[] = { "*.txt" };
	const char* file = tinyfd_openFileDialog("Файл", "", 1, filters, "Текстовые файлы", 0);

	if (!file) return;

	Rml::GetFileInterface()->LoadFile(file, m_sourceText);

	auto model = m_ctx->GetDataModel("form_model");
	model.GetModelHandle().DirtyVariable("sourceText");
}

void MainForm::ChangeAlphabet(Rml::Event& event)
{
	cip->alphabet = m_alphabetList[m_alphabet];
}

void MainForm::ChangeSeparator(Rml::Event& event)
{
	cip->separator = m_separator;
}

MainForm::MainForm(Rml::Context* ctx) : Form(ctx), m_keyword("КЛЮЧ"), m_separator(" "), 
m_sourceText("Текст"), m_result(""), m_alphabet(Alphabet::RUS)
{
	m_alphabetList[Alphabet::RUS] = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
	m_alphabetList[Alphabet::MIXED] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
										"abcdefghijklmnopqrstuvwxyz"
										"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"
										"абвгдеёжзийклмнопрстуфхцчшщъыьэюя"
										"0123456789 .,!?-:;()\"'";

	cip = std::make_unique<Cipher>(m_alphabetList[m_alphabet]);

	Rml::DataModelConstructor constructor = m_ctx->CreateDataModel("form_model");

	constructor.Bind("keyword", &m_keyword);
	constructor.Bind("separator", &m_separator);
	constructor.Bind("sourceText", &m_sourceText);
	constructor.Bind("result", &m_result);
	constructor.Bind("alphabet", &m_alphabet);

	m_doc = m_ctx->LoadDocumentFromMemory(
	R"-(
		<rml>
			<head>
				<title>Вариантный шифр</title>
				<style>
					scrollbarvertical {
						width: 0px;
					}

					div {
						display: block;
					}

					p {
						display: block;
					}

					h1 {
						display: block;
					}

					em {
						font-style: italic;
					}

					strong {
						font-weight: bold;
					}

					select {
						text-align: left;
					}

					tabset tabs {
						display: block;
					}

					table {
						box-sizing: border-box;
						display: table;
					}

					tr {
						box-sizing: border-box;
						display: table-row;
					}

					td {
						box-sizing: border-box;
						display: table-cell;
					}

					col {
						box-sizing: border-box;
						display: table-column;
					}

					colgroup {
						display: table-column-group;
					}

					thead,
					tbody,
					tfoot {
						display: table-row-group;
					}

					body {
						font-family: Noto Sans;
						padding: 16px;
						box-sizing: border-box;
						color: #222;
						background: #fefefe;
						width: 100vw;
						height: 100vh;
						overflow: scroll;
					}

					h1 {
						font-size: 32px;
						margin-bottom: 16px;
						text-align: center;
					}

					.panel {
						border: 1px #ddd;
						padding: 12px;
						box-sizing: border-box;
						border-radius: 8px;
						background: #fafafa;
						margin-bottom: 16px;
					}

					textarea {
						width: 100%;
						height: 80%;
						font-size: 24px;
						padding: 6px;
						border: 1px #ddd;
						border-radius: 8px;
						box-sizing: border-box;
						overflow: scroll;
					}

					label {
						display: block;
						margin: 6px 0 2px;
						font-weight: 600;
						font-size: 14px;
					}

					select {
						padding: 6px;
						font-size: 14px;
						border: 1px #ccc;
						border-radius: 4px;
						background: #fafafa;
						color: #222;
						min-width: 220px;
					}

					select option {
						font-size: 14px;
						padding: 2px;
						border: 1px #ccc;
						border-radius: 4px;
						color: #222;
						background: #fafafa;
					}

					input[type="text"] {
						padding: 6px;
						font-size: 14px;
						border: 1px #ccc;
						border-radius: 4px;
					}

					button {
						padding: .7em 1em;
						font-size: 14px;
						cursor: pointer;
						background: #f0f0f0;
						border: 1px #ccc;
						border-radius: 4px;

					}

					button:hover {
						background: #e6e6e6;
					}

					.controls {
						display: flex;
						flex-direction: column;
						gap: 8px;
						height: 30%;
						justify-content: center;
						align-items: stretch;
					}

					.row {
						display: flex;
						flex-wrap: wrap;
						align-items: center;
						gap: 8px;
						height: 30%;
					}

					.muted {
						color: #666;
						font-size: 13px;
					}

					.container {
						display: flex;
						flex-direction: row;
						gap: 16px;
						height: 60%;
						box-sizing: border-box;
					}

					.button-row {
						display: flex;
						flex-wrap: wrap;
						align-items: center;
						gap: 8px;
					}

					.columns {
						display: flex;
						flex-direction: row;
						flex-wrap: nowrap;
						gap: 16px;
						height: 100%;
						box-sizing: border-box;
					}

					.column {
						min-width: 280px;
					}
				</style>
			</head>

			<body data-model="form_model">
				<h1>Вариантный шифр</h1>
				<div class="panel controls">
					<div class="row">
						<label for="keyword">Ключ (keyword):</label>
						<input id="keyword" type="text" data-value="keyword" />
					</div>

					<div class="row">
						<label for="alphabetSelect">Алфавит:</label>
						<select id="alphabetSelect" data-value="alphabet">
							<option value="0" selected="selected">Только русский (А–Я, Ё)</option>
							<option value="1">Рус + Англ + цифры + символы</option>
						</select>
					</div>

					<div class="row">
						<label for="separator">Разделитель между парами:</label>
						<input id="separator" type="text" data-value="separator" style="width:60px;" />
						<span class="muted">Оставьте пробел или введите другой символ/строку</span>
					</div>

					<div class="row">
						<label for="file">Загрузить текстовый файл:</label>
						<button name="file" id="loadFileBtn">Загрузить</button>
					</div>
				</div>

				<div class="container">
					<div class="columns">
						<div class="panel column">
							<label for="inputText">Исходный текст:</label>
							<textarea id="inputText" data-value="sourceText"></textarea>
							<div class="button-row" style="margin-top:8px;">
								<button id="encryptBtn">Зашифровать</button>
								<button id="decryptBtn">Расшифровать</button>
							</div>
						</div>

						<div class="panel column">
							<label for="outputText">Результат:</label>
							<textarea id="outputText" data-value="result"></textarea>
							<div class="button-row" style="margin-top:8px;">
								<button id="copyBtn">Скопировать</button>
								<button id="saveBtn">Сохранить .txt</button>
							</div>
						</div>
					</div>
				</div>
			</body>
		</rml>
	)-");

	m_doc->GetElementById("encryptBtn")->AddEventListener(
		Rml::EventId::Click,
		new LambdaEventListener([this](Rml::Event& e) { EncryptText(e); }));

	m_doc->GetElementById("decryptBtn")->AddEventListener(
		Rml::EventId::Click,
		new LambdaEventListener([this](Rml::Event& e) { DecryptText(e); }));

	m_doc->GetElementById("copyBtn")->AddEventListener(
		Rml::EventId::Click,
		new LambdaEventListener([this](Rml::Event& e) { CopyText(e); }));

	m_doc->GetElementById("saveBtn")->AddEventListener(
		Rml::EventId::Click,
		new LambdaEventListener([this](Rml::Event& e) { SaveTextToFile(e); }));

	m_doc->GetElementById("loadFileBtn")->AddEventListener(
		Rml::EventId::Click,
		new LambdaEventListener([this](Rml::Event& e) { LoadTextFromFile(e); }));

	m_doc->GetElementById("alphabetSelect")->AddEventListener(
		Rml::EventId::Change,
		new LambdaEventListener([this](Rml::Event& e) { ChangeAlphabet(e); }));

	m_doc->GetElementById("separator")->AddEventListener(
		Rml::EventId::Change,
		new LambdaEventListener([this](Rml::Event& e) { ChangeSeparator(e); }));
}