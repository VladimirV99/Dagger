#pragma once

#include "core/core.h"
#include "core/graphics/window.h"
#include "core/system.h"

#include <imgui/imgui.h>

using namespace dagger;

#pragma region IMGUI DEBUG CONSOLE IMPLEMENTATION

class DebugConsole
{
	char m_InputBuf[256];
	ImVector<char*> m_Items;
	ImVector<const char*> m_Commands;
	ImVector<char*> m_History;
	int m_HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
	ImGuiTextFilter m_Filter;
	bool m_AutoScroll;
	bool m_ScrollToBottom;

	void TextEditCallbackCompletion(ImGuiInputTextCallbackData* data_);
	void TextEditCallbackHistory(ImGuiInputTextCallbackData* data_);

public:
	DebugConsole();
	~DebugConsole();

	// Portable helpers
	static int Stricmp(const char* s1_, const char* s2_);
	static int Strnicmp(const char* s1_, const char* s2_, int n_);
	static char* Strdup(const char* s_);
	static void Strtrim(char* s_);

	void ClearLog();
	void AddLog(const char* fmt_, ...);
	void Draw(const char* title_, bool* open_);
	void ExecCommand(const char* command_);

	static int TextEditCallbackStub(ImGuiInputTextCallbackData* data_)
	{
		auto* console = (DebugConsole*)data_->UserData;
		return console->TextEditCallback(data_);
	}

	int TextEditCallback(ImGuiInputTextCallbackData* data_);
};
#pragma endregion

class ConsoleSystem
	: public System
	, public Subscriber<GUIRender, Log>
{
	DebugConsole m_Console;
	bool m_IsOpen {false};
	void RenderGUI();
	void ReceiveLog(Log log_);

public:
	ConsoleSystem() = default;

	inline String SystemName() const override
	{
		return "Console System";
	}

	void SpinUp() override;
	void WindDown() override;
};