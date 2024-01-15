#include "Target.h"

#include <random>

Target::Target(sf::Vector2f position) 
	: particles(&shape.getPosition()) {
	shape.setPosition(position);
	shape.setRadius(radius);
	shape.setOrigin(radius, radius);
	shape.setOutlineThickness(5.f);

	particles.setSpawnRange(radius);
}

void Target::update(float dt) {
	particles.update(dt);
}
void Target::render(sf::RenderTarget* target) const {
	if (active)
		target->draw(shape);

	particles.render(target);
}

void Target::onHit(sf::Vector2f hitVel) {
	active = false;

	const float angleVariability = 3.141f / 2.f;
	for (int i = 0; i < particlesCount; i++) {
		//sf::Vector2f dir = hitVel;
		////dir /= sqrt(dir.x * dir.x + dir.y * dir.y);
		//float angle = atan2(dir.y, dir.x);
		//int angleGauss = rand() % 20 + rand() % 20 + rand() % 20 + rand() % 20 + rand() % 20;
		//angle += angleVariability * (float)(angleGauss) / 100.f - angleVariability / 2.f;
		//float power = 200 + rand() % 800;
		//dir.x = cos(angle);
		//dir.y = sin(angle);

		int col = rand() % 50;
		particles.emit(hitVel, sf::Color(col, col, col), 3.f);
	}
}

void Target::reset() {
	particles.reset();
	active = true;
}
