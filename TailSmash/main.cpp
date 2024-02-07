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
	sf::RenderWindow window(sf::VideoMode(16 * 130, 9 * 130), "Tail Smash!");
	sf::Clock clock;
	ImGui::SFML::Init(window, false);

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