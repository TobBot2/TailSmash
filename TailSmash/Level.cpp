#include "Level.h"
#include "Player.h"
#include "Target.h"
#include "Wall.h"

Level::Level(Player* player) : player(player) { }

void Level::setSpawn(sf::Vector2f position, float rotation) {
	spawnPoint = position;
	spawnRotation = rotation;
}

void Level::addTarget(sf::Vector2f position) {
	Target* t = new Target(position);
	targets.push_back(t);
}

void Level::addWall(sf::Vector2f topleft, sf::Vector2f size) {
	Wall* w = new Wall(topleft, size);
	walls.emplace_back(w);
}

void Level::update(float dt) {
	if (!complete)
		timer += dt;

	if (respawnTimer > 0) {
		respawnTimer -= dt;

		if (respawnTimer <= 0) {
			reset();
		}
	}

	bool targetsLeft = false;
	for (Target* t : targets) {
		if (t->isActive()) {
			targetsLeft = true;
		}
		t->update(dt); 	
	}
	if (!targetsLeft) {
		// WIN
		complete = true;
	}
}

void Level::render(sf::RenderTarget* target) const {
	for (Wall* w : walls) {
		w->render(target);
	}

	for (Target* t : targets) {
		t->render(target);
	}
}

void Level::playerDie() {
	respawnTimer = respawnTime;
}

void Level::reset() {
	complete = false;
	timer = 0;
	player->respawn(spawnPoint, spawnRotation);
	for (Target* t : targets) {
		t->reset();
	}
}

float Level::getScore() const {
	return timer; // maybe incorporate velocity of target hit too...
}
