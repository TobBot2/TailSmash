#pragma once

#include <SFML/Graphics.hpp>

class ParticleSystem {
public:
	ParticleSystem(const sf::Vector2f* position);

	void update(float dt);
	void render(sf::RenderTarget* target) const;

	void reset();
	void emit(sf::Vector2f vel, sf::Color color, float lifetime);

	void setSpawnRange(float range) { spawnRange = range; }
	void setVelVariability(int var) { velVariability = var; }
	void setAngleVariability(float angle, bool gaussian) { angleVariability = angle, gaussianAngle = gaussian; }
	void setFriction(float friction) { this->friction = friction; }
	void setDecay(float decayPercent) { decayAt = decayPercent; }
	void setForceField(sf::Vector2f field) { forceField = field; }

private:
	struct Particle {
		Particle(sf::Vector2f pos, sf::Vector2f vel, sf::Color color, float lifetime)
			: pos(pos), vel(vel), color(color), lifetime(lifetime) { };
		sf::Vector2f pos;
		sf::Vector2f vel;

		sf::Color color;

		float lifetime;
		float percent = 0.f;
	};

	const sf::Vector2f* position;

	float friction = .95f;
	sf::Vector2f forceField;
	float decayAt = .8f;

	float spawnRange = 0.f;
	int velVariability = 80;
	float angleVariability = 3.141f * .66f;
	bool gaussianAngle = true;

	sf::Color color;

	std::vector<Particle*> awakeParticles;
	std::vector<Particle*> sleepingParticles;
};

