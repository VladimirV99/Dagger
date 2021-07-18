#include "console.h"

#include "core/engine.h"

using namespace dagger;

#pragma region IMGUI DEBUG CONSOLE IMPLEMENTATION

DebugConsole::DebugConsole()
{
	ClearLog();
	memset(m_InputBuf, 0, sizeof(m_InputBuf));
	m_HistoryPos = -1;

	// "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
	m_Commands.push_back("HELP");
	m_Commands.push_back("HISTORY");
	m_Commands.push_back("CLEAR");
	m_Commands.push_back("CLASSIFY");
	m_AutoScroll = true;
	m_ScrollToBottom = false;
}

DebugConsole::~DebugConsole()
{
	ClearLog();
	for (int i = 0; i < m_History.Size; i++)
		free(m_History[i]);
}

// Portable helpers
int DebugConsole::Stricmp(const char* s1_, const char* s2_)
{
	int d;
	while ((d = toupper(*s2_) - toupper(*s1_)) == 0 && (*s1_ != 0))
	{
		s1_++;
		s2_++;
	}
	return d;
}

int DebugConsole::Strnicmp(const char* s1_, const char* s2_, int n_)
{
	int d = 0;
	while (n_ > 0 && (d = toupper(*s2_) - toupper(*s1_)) == 0 && (*s1_ != 0))
	{
		s1_++;
		s2_++;
		n_--;
	}
	return d;
}

char* DebugConsole::Strdup(const char* s_)
{
	size_t len = strlen(s_) + 1;
	void* buf = malloc(len);
	IM_ASSERT(buf);
	return (char*)memcpy(buf, (const void*)s_, len);
}

void DebugConsole::Strtrim(char* s_)
{
	char* strEnd = s_ + strlen(s_);
	while (strEnd > s_ && strEnd[-1] == ' ')
		strEnd--;
	*strEnd = 0;
}

void DebugConsole::ClearLog()
{
	for (int i = 0; i < m_Items.Size; i++)
		free(m_Items[i]);
	m_Items.clear();
}

void DebugConsole::AddLog(const char* fmt_, ...)
{
	// FIXME-OPT
	char buf[1024];
	va_list args;
	va_start(args, fmt_);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt_, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	m_Items.push_back(Strdup(buf));
}

void DebugConsole::Draw(const char* title_, bool* open_ = nullptr)
{
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(title_, open_))
	{
		ImGui::End();
		return;
	}

	// Reserve enough left-over height for 1 separator + 1 input text
	const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	ImGui::BeginChild(
		"ScrollingRegion", ImVec2(0, -footerHeightToReserve), false, ImGuiWindowFlags_HorizontalScrollbar);
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::Selectable("Clear"))
			ClearLog();
		ImGui::EndPopup();
	}

	// Display every line as a separate entry so we can change their color or add custom widgets.
	// If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
	// NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
	// to only process visible items. The clipper will automatically measure the height of your first item and then
	// "seek" to display only items in the visible area.
	// To use the clipper we can replace your standard loop:
	//      for (int i = 0; i < Items.Size; i++)
	//   With:
	//      ImGuiListClipper clipper;
	//      clipper.Begin(Items.Size);
	//      while (clipper.Step())
	//         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
	// - That your items are evenly spaced (same height)
	// - That you have cheap random access to your elements (you can access them given their index,
	//   without processing all the ones before)
	// You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
	// We would need random-access on the post-filtered list.
	// A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
	// or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
	// and appending newly elements as they are inserted. This is left as a task to the user until we can manage
	// to improve this example code!
	// If your items are of variable height:
	// - Split them into same height items would be simpler and facilitate random-seeking into your list.
	// - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
	for (int i = 0; i < m_Items.Size; i++)
	{
		const char* item = m_Items[i];
		if (!m_Filter.PassFilter(item))
			continue;

		// Normally you would store more information in your item than just a string.
		// (e.g. make Items[] an array of structure, store color/type etc.)
		ImVec4 color;
		bool hasColor = false;
		if (strstr(item, "[error]") != nullptr)
		{
			color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
			hasColor = true;
		}
		else if (strncmp(item, "# ", 2) == 0)
		{
			color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
			hasColor = true;
		}
		if (hasColor)
			ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(item);
		if (hasColor)
			ImGui::PopStyleColor();
	}

	if (m_ScrollToBottom || (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
		ImGui::SetScrollHereY(1.0f);
	m_ScrollToBottom = false;

	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::Separator();

	// Command-line
	bool reclaimFocus = false;
	ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion |
										 ImGuiInputTextFlags_CallbackHistory;
	if (ImGui::InputText("", m_InputBuf, IM_ARRAYSIZE(m_InputBuf), inputTextFlags, &TextEditCallbackStub, (void*)this))
	{
		char* s = m_InputBuf;
		Strtrim(s);
		if (s[0] != '\0')
			ExecCommand(s);
		strncpy(s, "", 2);
		reclaimFocus = true;
	}

	// Auto-focus on window apparition
	ImGui::SetItemDefaultFocus();
	if (reclaimFocus)
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

	ImGui::End();
}

void DebugConsole::ExecCommand(const char* command_)
{
	AddLog("# %s\n", command_);

	m_HistoryPos = -1;
	for (int i = m_History.Size - 1; i >= 0; i--)
		if (Stricmp(m_History[i], command_) == 0)
		{
			free(m_History[i]);
			m_History.erase(m_History.begin() + i);
			break;
		}
	m_History.push_back(Strdup(command_));

	//    Engine::Dispatcher().trigger<Command>(Command(command_));

	m_ScrollToBottom = true;
}

void DebugConsole::TextEditCallbackCompletion(ImGuiInputTextCallbackData* data_)
{
	// Example of TEXT COMPLETION

	// Locate beginning of current word
	const char* wordEnd = data_->Buf + data_->CursorPos;
	const char* wordStart = wordEnd;
	while (wordStart > data_->Buf)
	{
		const char c = wordStart[-1];
		if (c == ' ' || c == '\t' || c == ',' || c == ';')
			break;
		wordStart--;
	}

	// Build a list of candidates
	ImVector<const char*> candidates;
	for (int i = 0; i < m_Commands.Size; i++)
		if (Strnicmp(m_Commands[i], wordStart, (int)(wordEnd - wordStart)) == 0)
			candidates.push_back(m_Commands[i]);

	if (candidates.Size == 0)
	{
		// No match
		AddLog("No match for \"%.*s\"!\n", (int)(wordEnd - wordStart), wordStart);
	}
	else if (candidates.Size == 1)
	{
		// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
		data_->DeleteChars((int)(wordStart - data_->Buf), (int)(wordEnd - wordStart));
		data_->InsertChars(data_->CursorPos, candidates[0]);
		data_->InsertChars(data_->CursorPos, " ");
	}
	else
	{
		// Multiple matches. Complete as much as we can..
		// So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
		int matchLen = (int)(wordEnd - wordStart);
		for (;;)
		{
			int c = 0;
			bool allCandidatesMatches = true;
			for (int i = 0; i < candidates.Size && allCandidatesMatches; i++)
				if (i == 0)
					c = toupper(candidates[i][matchLen]);
				else if (c == 0 || c != toupper(candidates[i][matchLen]))
					allCandidatesMatches = false;
			if (!allCandidatesMatches)
				break;
			matchLen++;
		}

		if (matchLen > 0)
		{
			data_->DeleteChars((int)(wordStart - data_->Buf), (int)(wordEnd - wordStart));
			data_->InsertChars(data_->CursorPos, candidates[0], candidates[0] + matchLen);
		}

		// List matches
		AddLog("Possible matches:\n");
		for (int i = 0; i < candidates.Size; i++)
			AddLog("- %s\n", candidates[i]);
	}
}

void DebugConsole::TextEditCallbackHistory(ImGuiInputTextCallbackData* data_)
{
	// Example of HISTORY
	const int prevHistoryPos = m_HistoryPos;
	if (data_->EventKey == ImGuiKey_UpArrow)
	{
		if (m_HistoryPos == -1)
			m_HistoryPos = m_History.Size - 1;
		else if (m_HistoryPos > 0)
			m_HistoryPos--;
	}
	else if (data_->EventKey == ImGuiKey_DownArrow)
	{
		if (m_HistoryPos != -1)
			if (++m_HistoryPos >= m_History.Size)
				m_HistoryPos = -1;
	}

	// A better implementation would preserve the data on the current input line along with cursor position.
	if (prevHistoryPos != m_HistoryPos)
	{
		const char* historyStr = (m_HistoryPos >= 0) ? m_History[m_HistoryPos] : "";
		data_->DeleteChars(0, data_->BufTextLen);
		data_->InsertChars(0, historyStr);
	}
}

int DebugConsole::TextEditCallback(ImGuiInputTextCallbackData* data_)
{
	// AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
	switch (data_->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackCompletion:
	{
		TextEditCallbackCompletion(data_);
		break;
	}
	case ImGuiInputTextFlags_CallbackHistory:
	{
		TextEditCallbackHistory(data_);
		break;
	}
	}
	return 0;
}

#pragma endregion

void ConsoleSystem::RenderGUI()
{
	m_Console.Draw("Console");
}

void ConsoleSystem::ReceiveLog(Log log_)
{
	m_Console.AddLog(log_.message.c_str());
}

void ConsoleSystem::SpinUp()
{
	Engine::Dispatcher().sink<GUIRender>().connect<&ConsoleSystem::RenderGUI>(this);
	Engine::Dispatcher().sink<Log>().connect<&ConsoleSystem::ReceiveLog>(this);
}

void ConsoleSystem::WindDown()
{
	Engine::Dispatcher().sink<GUIRender>().disconnect<&ConsoleSystem::RenderGUI>(this);
	Engine::Dispatcher().sink<Log>().disconnect<&ConsoleSystem::ReceiveLog>(this);
}
