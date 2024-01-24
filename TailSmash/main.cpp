#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

#include "Player.h"
#include "Manager.h"

// https://www.sfml-dev.org/tutorials/2.5
int main()
{
	const unsigned int pixelSize = 5;

	sf::RenderWindow window(sf::VideoMode(16 * 130, 9 * 130), "Tail Smash!");
	sf::View view = window.getDefaultView(); // stores copy of view (2d camera)
	sf::Clock clock;

	sf::RenderTexture lowresWindow;
	lowresWindow.create(window.getSize().x / pixelSize, window.getSize().y / pixelSize);
	lowresWindow.setSmooth(false);
	sf::Sprite lowresSprite(lowresWindow.getTexture());
	lowresSprite.setScale(sf::Vector2f(pixelSize, pixelSize));

	sf::Font font;
	if (!font.loadFromFile("resources/Retroica.ttf")) {
		std::cout << "error loading font resources/Retroica.ttf\n";
	}

	sf::Music music;
	if (!music.openFromFile("resources/musicLD38T9.wav")) {
		std::cout << "error loading background music resources/musicLD38T9.wav\n";
	}
	music.setLoop(true);
	music.play();

	sf::Text fpsText("", font);
	fpsText.setFillColor(sf::Color(0xe9efecff));
	fpsText.setOutlineColor(sf::Color(0x211e20ff));
	fpsText.setOutlineThickness(3.f);
	fpsText.setPosition(0, 0);
	const float updateFpsTextTime = .5f;
	float updateFpsTextTimer = updateFpsTextTime;

	Player player(sf::Vector2f(window.getSize()) / 2.f, sf::Vector2f(60.f, 35.f));
	Manager manager(&player, sf::Vector2f(window.getSize()), font);

	player.setManager(&manager);

	window.setVerticalSyncEnabled(false);
	window.setFramerateLimit(60);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
				/*case sf::Event::Resized:
					view.reset(sf::FloatRect(0.f, 0.f, (float) event.size.width, (float) event.size.height));
					window.setView(view);
					break;*/
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Escape:
					window.close();
					break;
				}
				break;
			default:
				break;
			}
		}

		// update
		float dt = clock.restart().asSeconds();

		updateFpsTextTimer -= dt;
		if (updateFpsTextTimer < 0) {
			fpsText.setString("framerate: " + std::to_string((int)(1.f / dt)));
			updateFpsTextTimer = updateFpsTextTime;
		}

		manager.update(dt);

		//view.setCenter(player.getShape().getPosition());
		lowresWindow.setView(view);

		// render
		lowresWindow.clear(sf::Color(0x555568ee));

		manager.render(&lowresWindow);

		lowresWindow.display();

		window.draw(lowresSprite, &manager.getShader());
		// ui
		window.draw(fpsText);

		manager.renderUI(&window);

		window.display();
	}

	return 0;
}