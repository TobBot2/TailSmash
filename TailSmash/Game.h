#pragma once
#include <SFML/Graphics.hpp>

#include "Player.h"
#include "Manager.h"

class Game {
public:
	enum GameState {
		Menu,
		Settings,
		Credits,
		Play,
		Pause
	};

	Game(sf::View view);
	void update(float elapsedTime);
	void render(sf::RenderTarget* target);

private:
	static const int pixelSize = 5;

	GameState state = GameState::Play;

	Player player;
	Manager manager;

	sf::View view;
	sf::RenderTexture lowresWindow;
	sf::Sprite lowresSprite;

	sf::Font font;
	sf::Music music;

	sf::Text fpsText;
	const float updateFpsTextTime = .5f;
	float updateFpsTextTimer = updateFpsTextTime;
};

