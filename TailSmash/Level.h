#pragma once

#include <SFML/Graphics.hpp>

class Target;
class Wall;
class Player;
class Level {
public:
	void setPlayer(Player* player);
	void setSpawn(sf::Vector2f position, float rotation);
	void setName(std::string name);
	void addTarget(sf::Vector2f position);
	void addWall(sf::Vector2f topleft, sf::Vector2f size);

	void update(float dt);
	void render(sf::RenderTarget* target) const;

	void playerDie();
	void reset();
	float getScore() const;
	float getHighScore() const;
	void setHighScore(float score);

	sf::Vector2f getSpawnPos() const { return spawnPoint; }
	float getSpawnRot() const { return spawnRotation; }
	std::string getName() const { return name; }

	bool isComplete() const { return complete; };

	std::vector<Target*>& getTargets() { return targets; }
	std::vector<Wall*>& getWalls() { return walls; }

private:
	Player* player;

	bool complete = false;

	std::vector<Target*> targets;
	std::vector<Wall*> walls;

	std::string name;

	float timer = 0.f;
	float highscore = -1.f;

	float respawnTime = .3f;
	float respawnTimer = 0;

	sf::Vector2f spawnPoint;
	float spawnRotation = 0.f;
};

