#pragma once

#include <SFML/Graphics.hpp>

#include "ParticleSystem.h"

class Target {
public:
	Target(sf::Vector2f position);

	void update(float dt);
	void render(sf::RenderTarget* target) const;

	void onHit(sf::Vector2f hitOrigin);
	void reset();

	bool isActive() const { return active; }

	sf::CircleShape& getShape() { return shape; }
private:
	sf::CircleShape shape;
	float radius = 15.f;

	int particlesCount = 300;
	ParticleSystem particles;

	bool active = true;
};

