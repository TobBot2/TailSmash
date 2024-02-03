#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Player;
class Level;
class Manager {
public:
	Manager(sf::Vector2f gameSize);
	void setPlayer(Player* player);
	void setFont(sf::Font& font);

	void setScoreNormal();
	void setScoreFinish();

	void update(float dt);

	void render(sf::RenderTarget* target) const;
	void renderUI(sf::RenderTarget* target) const;

	void shockwave(sf::Vector2f position, float velSq);
	const sf::Shader& getShader() const { return globalShader; }
private:
	Player* player;
	std::vector<Level*> levels;

	float timeScale = 1.f;
	float globalTime = 0.f;

	sf::Text scoreText;
	float scoreTextSize = 90.f;
	float scoreTextOutlineSize = 9.f;
	float scoreTextFinishSize = 180.f;
	float scoreTextFinishOutlineSize = 18.f;

	int activeLevel = 0;

	float waitAfterLevelTime = 2.f;
	float waitAfterLevelTimer = 0.f;

	float slomoTimescale = 0;
	float slomoTime = .05f;
	float slomoTimer = 0.f;

	sf::SoundBuffer expSml;
	sf::SoundBuffer expMed;
	sf::SoundBuffer expBig;

	sf::Sound barrelSml;
	sf::Sound barrelMed;
	sf::Sound barrelBig;

	sf::Vector2f gameSize;
	sf::Shader globalShader;
};

