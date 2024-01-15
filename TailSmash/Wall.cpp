#include "Wall.h"

Wall::Wall(sf::Vector2f position, sf::Vector2f size) {
	shape.setPosition(position + size / 2.f);
	shape.setSize(size);
	shape.setOrigin(size / 2.f);
	shape.setFillColor(sf::Color(160, 220, 160));
	shape.setOutlineThickness(5.f);
	shape.setOutlineColor(sf::Color::Black);
}

void Wall::render(sf::RenderTarget* target) const {
	target->draw(shape);
}