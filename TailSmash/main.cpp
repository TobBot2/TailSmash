#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>


#include "Game.h"
// https://www.sfml-dev.org/tutorials/2.5
int main()
{
	sf::Vector2u optionsSize(fmin(sf::VideoMode::getDesktopMode().width * .8f, 800), fmin(sf::VideoMode::getDesktopMode().height * .8f, 800));
	sf::RenderWindow displayOptions(sf::VideoMode(optionsSize.x * .8f, optionsSize.y * .8f), "Tail Smash Launcher");
	sf::Clock optsClock;
	ImGui::SFML::Init(displayOptions, false);
	ImGuiIO& optsIo = ImGui::GetIO();
	optsIo.Fonts->AddFontFromFileTTF("resources/mont.otf", optionsSize.y / 30.f);
	ImGui::SFML::UpdateFontTexture();
	int resolutionChosen = 2;
	std::vector<const char*> resolutions = { /*"Fullscreen",*/"3840x2160", "2560x1440", "2080x1170", "1920x1080", "1280x720", "848x480", "480x272"};
	while (displayOptions.isOpen()) {
		sf::Event event;
		while (displayOptions.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(displayOptions, event);
			switch (event.type) {
			case sf::Event::Closed:
				displayOptions.close();
				ImGui::SFML::Shutdown();
				return 0;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Escape:
					displayOptions.close();
					ImGui::SFML::Shutdown();
					return 0;
				default:
					displayOptions.close();
					break;
				}
			}
		}

		ImGui::SFML::Update(displayOptions, optsClock.restart());

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(displayOptions.getSize()));
		ImGui::Begin("Tail Smash Launcher", nullptr, 
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

		ImGui::Combo("Resolution", &resolutionChosen, &resolutions[0], resolutions.size());
		ImGui::TextWrapped("Note that you can resize the game by dragging the bottom right corner of the window");

		if (ImGui::Button("Launch", ImVec2(optionsSize.x * .5f, optionsSize.y * .2f))) {
			displayOptions.close();
		}

		ImGui::End();

		ImGui::SFML::Render(displayOptions);
		displayOptions.display();
	}

	sf::VideoMode mode = sf::VideoMode(16 * 130, 9 * 130);
	int flag = sf::Style::Default;
	/*if (resolutionChosen == 0) {
		mode = sf::VideoMode::getFullscreenModes()[0];
		flag = sf::Style::Fullscreen;
	}*/

	sf::RenderWindow window(mode, "Tail Smash!", flag);
	sf::Clock clock;
	ImGui::SFML::Init(window, false);

	if (resolutionChosen != 0) {
		std::string resolutionStr = resolutions[resolutionChosen];
		size_t split = resolutionStr.find("x");
		window.setSize(sf::Vector2u(atoi(resolutionStr.substr(0, split).c_str()), atoi(resolutionStr.substr(split + 1, resolutionStr.length()).c_str())));
	}

	// set fonts
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("resources/mont.otf", window.getSize().y / 20.f);
	ImGui::SFML::UpdateFontTexture();

	// Set ImGui style options for buttons
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Button] = ImVec4(.63f, .63f, 0.55f, 1.0f); // Button color
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.58f, 0.58f, 0.5f, 1.0f); // Button hover color
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.91f, 0.94f, 0.93f, 1.0f); // Button pressed color
	style.Colors[ImGuiCol_Border] = ImVec4(0.13f, 0.13f, 0.13f, 1.f);
	style.FrameBorderSize = 5.0f; // Border size
	style.FrameRounding = 3.0f; // Border rounding

	Game game(window.getDefaultView());

	window.setVerticalSyncEnabled(false);
	window.setFramerateLimit(60);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(window, event);
			switch (event.type) {
			case sf::Event::Closed:
				game.saveScores();
				window.close();
				break;
				/*case sf::Event::Resized:
					view.reset(sf::FloatRect(0.f, 0.f, (float) event.size.width, (float) event.size.height));
					window.setView(view);
					break;*/
			case sf::Event::KeyPressed:
				/*switch (event.key.code) {
				case sf::Keyboard::Escape:
					window.close();
					break;
				}*/
				break;
			default:
				break;
			}
		}
		sf::Time dt = clock.restart();
		ImGui::SFML::Update(window, dt);

		game.update(dt.asSeconds());
		
		game.render(&window);

		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
	return 0;
}