#pragma once
#include <SFML/Graphics.hpp>

#include "Player.h"
#include "Manager.h"

class Game {
public:
	enum GameState {
		Splash,
		Menu,
		LevelSelect,
		Settings,
		Credits,
		Play,
		Pause
	};

	Game(sf::View view);
	void update(float elapsedTime);
	void render(sf::RenderWindow* target);

	void saveScores();

private:
	static const int pixelSize = 5;

	GameState state = GameState::Splash;

	float splashDelay = 3.f;

	Player player;
	Manager manager;

	sf::View view;
	sf::RenderTexture lowresWindow;
	sf::Sprite lowresSprite;

	sf::Font font;
	sf::Music music;

	float musicVolume = 50.f;
	float sfxVolume = 50.f;

	std::vector<float> scores;
	float netHighScore = 0.f;

	sf::Text fpsText;
	const float updateFpsTextTime = .5f;
	float updateFpsTextTimer = updateFpsTextTime;
};

