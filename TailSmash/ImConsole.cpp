#include "ImConsole.h"

#include <iostream>
#include <string>

ImConsole::ImConsole(std::string text, std::vector<std::string> history)
{
	input = text;
	this->history = history;
}

std::pair<std::string, bool> ImConsole::Draw(const char* label, ImVec2 size) {
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(size);
	ImGui::Begin(label, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

	ImGui::SetCursorPos(ImVec2(10, 10));
	if (ImGui::Button("Clear history")) {
		history.clear();
	}

	for (std::string entry : history) {
		ImGui::Text(entry.c_str());
	}

	int flags = ImGuiInputTextFlags_EnterReturnsTrue;
	bool commandUsed = false;

	char buf[256];
	strncpy_s(buf, input.c_str(), sizeof(buf) - 1);
	ImGui::SetCursorPosY(size.y - ImGui::GetFontSize() * 1.1f);
	if (ImGui::InputText("command", buf, sizeof(buf), flags)) {
		commandUsed = true;
	}
	input = buf;

	ImGui::End();

	return std::make_pair(input, commandUsed);
}
