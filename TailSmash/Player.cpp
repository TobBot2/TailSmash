#include <iostream>

#include "Player.h"
#include "Target.h"
#include "Wall.h"

Player::Player(sf::Vector2f pos, sf::Vector2f size)
	: particles(&bodyShape.getPosition()) {
	bodyShape.setPosition(pos);
	bodyShape.setSize(size);
	bodyShape.setOrigin(size / 2.f);
	bodyShape.setFillColor(sf::Color(200, 200, 200));
	bodyShape.setOutlineThickness(5.f);
	bodyShape.setOutlineColor(sf::Color::Black);
	bodyShape.setRotation(90.f);

	tailShape.setRadius(tailRadius);
	tailShape.setOrigin(tailRadius, tailRadius);
	tailShape.setFillColor(sf::Color(180, 180, 220));
	tailShape.setOutlineThickness(5.f);
	tailShape.setOutlineColor(sf::Color::Black);

	particles.setDecay(0.f);
	particles.setAngleVariability(3.141f / 8.f, true);
	particles.setFriction(.98f);
	particles.setSpawnRange(size.y / 2.f);

	generateChain();
}

void Player::setLevel(Level* level) { this->level = level; }
void Player::disable() { alive = false; }

void Player::update(float dt) {
	particles.update(dt);
	float speedPercent = abs(vel) / maxVel;

	sf::Vector2f input = alive? getInput() : sf::Vector2f(0.f, 0.f);

	bodyShape.rotate(input.x * steerAcc * sqrt(speedPercent)*dt);
	float rads = bodyShape.getRotation() * 3.141f / 180.f;

	if (abs(vel) > 0.f) {
		vel -= std::min(friction * dt, abs(vel)) * vel / abs(vel);
		for (int i = 0; i < 10; i++) {
			particles.emit(sf::Vector2f(-vel, -vel) * 30000.f, sf::Color(70, 40, 35, 100 + rand() % 100), .3f);
		}
	}
	if (speedPercent <= 1.f) {
		vel += input.y * acc * dt;
	}

	bodyShape.move(cos(rads) * vel * dt, sin(rads) * vel * dt);

	updateChains(dt);

	checkCollision();
}

void Player::render(sf::RenderTarget* target) const {
	particles.render(target);

	int index = 0;
	sf::VertexArray chainVerts(sf::PrimitiveType::LineStrip, chainNodes.size() + 1);
	chainVerts[index++] = sf::Vertex(bodyShape.getPosition(), sf::Color::Black);
	for (const ChainNode* n : chainNodes) {
		chainVerts[index++] = sf::Vertex(n->pos, sf::Color::Black);
	}
	target->draw(chainVerts);

	target->draw(bodyShape);
	target->draw(tailShape);
}

void Player::respawn(sf::Vector2f position, float rotation) {
	bodyShape.setPosition(position);
	bodyShape.setRotation(rotation);
	bodyShape.setFillColor(sf::Color(200, 200, 200));
	vel = 0.f;

	for (ChainNode* c : chainNodes) {
		delete c;
	}
	chainNodes.clear();

	alive = true;

	generateChain();
}

const sf::RectangleShape& Player::getShape() const {
	return bodyShape;
}

sf::Vector2f Player::getInput() {
	sf::Vector2f axis(0.f, 0.f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) axis.y -= 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) axis.x -= 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) axis.y += 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) axis.x += 1;

	float mag = sqrtf(axis.x * axis.x + axis.y * axis.y);
	if (mag <= 1.f) return axis;
	return axis / mag;
}

void Player::checkCollision() {
	if (level == nullptr) return;
	if (!alive) return;

	for (Wall* w : level->getWalls()) {
		sf::FloatRect wallBounds = w->getShape().getGlobalBounds();
		float rads = bodyShape.getRotation() * 3.141f / 180.f;
		sf::Vector2f sides(bodyShape.getSize().x * cos(rads), bodyShape.getSize().y * sin(rads));
		
		if (wallBounds.contains(bodyShape.getPosition() - sides / 2.f) ||
			wallBounds.contains(bodyShape.getPosition() + sides / 2.f) ||
			wallBounds.contains(bodyShape.getPosition() + sf::Vector2f(sides.x / 2.f, -sides.y / 2.f)) ||
			wallBounds.contains(bodyShape.getPosition() + sf::Vector2f(-sides.x / 2.f, sides.y / 2.f))) {
			// HIT WALL
			bodyShape.setFillColor(sf::Color::Red);
			vel *= -.4f;
			alive = false;
			level->playerDie();
			break;
		}
	}

	for (Target* t : level->getTargets()) {
		if (!t->isActive()) continue;

		sf::Vector2f targetPos = t->getShape().getPosition();
		float targetRadius = t->getShape().getRadius();

		sf::Vector2f diff = targetPos - *tailPos;
		float distanceSq = diff.x * diff.x + diff.y * diff.y;
		
		if (distanceSq < (targetRadius + tailRadius) * (targetRadius + tailRadius)) {
			// HIT TARGET
			t->onHit(*(tailPos + 1)); // tailPos + 1 is vel
		}
	}
}

void Player::generateChain() {
	float segLen = tailLen / (float)tailSegments;
	float rads = bodyShape.getRotation() * 3.141f / 180.f;
	sf::Vector2f back(cos(rads) * segLen, sin(rads) * segLen);
	ChainNode* node = new ChainNode(nullptr, bodyShape.getPosition() + back);
	chainNodes.push_back(node);
	const sf::Vector2f* parentPos = &node->pos;
	for (int i = 1; i < tailSegments; i++) {
		node = new ChainNode(parentPos, *parentPos + back);
		chainNodes.push_back(node);
		parentPos = &node->pos;
	}
	tailPos = &node->pos;
}

void Player::updateChains(float dt) {
	float segLen = tailLen / (float)chainNodes.size();
	for (ChainNode* n : chainNodes) {
		n->pos += n->vel;

		if (abs(n->vel.x) > 0.f || abs(n->vel.y) > 0.f) {
			float nVelLen = sqrt(n->vel.x * n->vel.x + n->vel.y * n->vel.y);
			if (n->friction * dt > nVelLen) {
				n->vel = {};
			}
			else {
				n->vel -= n->vel / nVelLen * n->friction * dt;
			}
		}

		sf::Vector2f parentPos;
		if (n->parentPos == nullptr) {
			parentPos = bodyShape.getPosition();
		}
		else {
			parentPos = *n->parentPos;
		}

		float a = n->pos.x - parentPos.x;
		float b = n->pos.y - parentPos.y;
		float len = sqrt(a * a + b * b);

		if (len > segLen) {
			sf::Vector2f diff(a * segLen / len, b * segLen / len);
			n->vel += parentPos + diff - n->pos;
			n->pos = parentPos + diff;
		}
	}

	tailShape.setPosition(*tailPos);
}