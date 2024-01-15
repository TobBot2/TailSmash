#pragma once

#include <SFML/Graphics.hpp>

#include "Level.h"
#include "ParticleSystem.h"

class Player {
public:
	Player(sf::Vector2f pos, sf::Vector2f size);

	void setLevel(Level* level);

	void update(float dt);
	void render(sf::RenderTarget* target) const;

	void respawn(sf::Vector2f position, float rotation);
	void disable();

	const sf::RectangleShape& getShape() const;
private:
	struct ChainNode {
		ChainNode(const sf::Vector2f* parentPos, sf::Vector2f pos)
			: parentPos(parentPos), pos(pos) { };
		const sf::Vector2f* parentPos;

		sf::Vector2f pos;
		sf::Vector2f vel;

		float friction = 120.f;
	};

	void generateChain();

	sf::Vector2f getInput();
	void checkCollision();
	void updateChains(float dt);

	Level* level = nullptr;

	float acc = 1000.f;
	float steerAcc = 320.f; // in rads/s^2
	float vel = 0.f;
	float maxVel = 900.f;
	float friction = 240.f;

	bool alive = true;

	std::vector<ChainNode*> chainNodes;

	float tailSegments = 5;
	float tailLen = 300.f;
	sf::Vector2f* tailPos;
	float tailRadius = 50.f;

	sf::RectangleShape bodyShape;
	sf::CircleShape tailShape;

	ParticleSystem particles;
};

