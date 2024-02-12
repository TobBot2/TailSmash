#include "imgui.h"
#include "imgui-SFML.h"

#include <vector>

class ImConsole {
public:
	ImConsole(std::string text, std::vector<std::string> history);
	std::pair<std::string, bool> Draw(const char* label, ImVec2 size);

	std::vector<std::string> getHistory() { return history; }
private:
	std::vector<std::string> history;
	std::string input;
};