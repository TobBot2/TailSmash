#include "Wall.h"

Wall::Wall(sf::Vector2f position, sf::Vector2f size) {
	shape.setPosition(position + size / 2.f);
	shape.setSize(size);
	shape.setOrigin(size / 2.f);
	shape.setFillColor(sf::Color(0x211e20ff));
	shape.setOutlineThickness(5.f);
	shape.setOutlineColor(sf::Color(0xa0a08bff));
}

void Wall::render(sf::RenderTarget* target) const {
	target->draw(shape);
}