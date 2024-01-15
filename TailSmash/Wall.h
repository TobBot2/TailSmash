#pragma once

#include <SFML/Graphics.hpp>

class Wall {
public:
	Wall(sf::Vector2f position, sf::Vector2f size);

	void render(sf::RenderTarget* target) const;

	sf::RectangleShape& getShape() { return shape; };
private:
	sf::RectangleShape shape;
};

