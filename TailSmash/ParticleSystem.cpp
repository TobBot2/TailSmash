#include "ParticleSystem.h"

#include <iostream>

ParticleSystem::ParticleSystem(const sf::Vector2f* position) 
	: position(position) { }

void ParticleSystem::update(float dt) {
	//std::cout << "\n...";
	for (Particle* p : awakeParticles) {
		p->pos += (p->vel + forceField) * dt;
		p->vel *= friction;

		p->percent += dt / p->lifetime;

		if (p->percent > decayAt) {
			int alpha = 255 * (decayAt - p->percent) / (1.f - decayAt);
			p->color = sf::Color(p->color.r, p->color.g, p->color.b, alpha);
		}

		if (p->percent > 1.f) {
			sleepingParticles.push_back(p);
			// gets removed from awakeParticles just after loop ends...
		}
	}
	awakeParticles.erase(std::remove_if(awakeParticles.begin(), awakeParticles.end(),
		[](Particle* p) { return p->percent > 1.f; }), awakeParticles.end());
}

void ParticleSystem::render(sf::RenderTarget* target) const {
	sf::VertexArray vertices(sf::Points);
	for (Particle* p : awakeParticles) {
		vertices.append(sf::Vertex(p->pos, p->color));
	}
	
	target->draw(vertices);
}


void ParticleSystem::reset() {
	for (Particle* p : awakeParticles) {
		delete p;
	}
	for (Particle* p : sleepingParticles) {
		if (p != nullptr) delete p;
	}
	awakeParticles.clear();
	sleepingParticles.clear();
}

void ParticleSystem::emit(sf::Vector2f vel, sf::Color color, float lifetime) {
	sf::Vector2f dir = vel;
	float mag = sqrt(dir.x * dir.x + dir.y * dir.y);
	float angle = atan2(dir.y, dir.x);
	int randAngle = gaussianAngle
		? rand() % 20 + rand() % 20 + rand() % 20 + rand() % 20 + rand() % 20
		: rand() % 100;
	angle += angleVariability * ((float)(randAngle) / 100.f - .5f);
	float power = rand() % velVariability;
	dir.x = cos(angle) * power * mag;
	dir.y = sin(angle) * power * mag;

	sf::Vector2f pos = *position;
	float randPosAngle = 6.282f * (float)(rand() % 100) / 100.f;
	pos += sf::Vector2f(cos(randPosAngle) * spawnRange, sin(randPosAngle) * spawnRange);

	if (sleepingParticles.empty()) {
		// create new particle
		Particle* p = new Particle(pos, dir, color, lifetime);
		awakeParticles.push_back(p);
	}
	else {
		Particle* p = sleepingParticles.back();
		sleepingParticles.pop_back();
		p->pos = pos;
		p->vel = dir;
		p->color = color;
		p->lifetime = lifetime;
		p->percent = 0.f;
		awakeParticles.push_back(p);
	}
}

