#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

#include "Game.h"
// https://www.sfml-dev.org/tutorials/2.5
int main()
{
	sf::RenderWindow window(sf::VideoMode(16 * 130, 9 * 130), "Tail Smash!");
	sf::Clock clock;

	Game game(window.getDefaultView());

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

		game.update(clock.restart().asSeconds());
		
		game.render(&window);

		window.display();
	}

	return 0;
}