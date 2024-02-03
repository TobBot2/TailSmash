#include <iostream>
#include <SFML/Audio.hpp>

#include "Game.h"

Game::Game(sf::View view) :
	player(sf::Vector2f(view.getSize()) / 2.f, sf::Vector2f(60.f, 35.f)), manager(sf::Vector2f(view.getSize())) {
	
	this->view = view;
	lowresWindow.create(view.getSize().x / pixelSize, view.getSize().y / pixelSize);
	lowresWindow.setSmooth(false);

	lowresSprite = sf::Sprite(lowresWindow.getTexture());
	lowresSprite.setScale(sf::Vector2f(pixelSize, pixelSize));

	if (!font.loadFromFile("resources/Retroica.ttf")) {
		std::cout << "error loading font resources/Retroica.ttf\n";
	}

	if (!music.openFromFile("resources/musicLD38T9.wav")) {
		std::cout << "error loading background music resources/musicLD38T9.wav\n";
	}
	music.setLoop(true);
	music.play();

	fpsText = sf::Text("", font);
	fpsText.setFillColor(sf::Color(0xe9efecff));
	fpsText.setOutlineColor(sf::Color(0x211e20ff));
	fpsText.setOutlineThickness(3.f);
	fpsText.setPosition(0, 0);

	manager.setFont(font);
	manager.setPlayer(&player);
	player.setManager(&manager);
}

void Game::update(float elapsedTime) {
	updateFpsTextTimer -= elapsedTime;
	if (updateFpsTextTimer < 0) {
		fpsText.setString("framerate: " + std::to_string((int)(1.f / elapsedTime)));
		updateFpsTextTimer = updateFpsTextTime;
	}

	if (state == GameState::Play) {
		manager.update(elapsedTime);
	}
}

void Game::render(sf::RenderTarget* target) {
	lowresWindow.setView(view);

	// render
	lowresWindow.clear(sf::Color(0x555568ee));

	if (state == GameState::Play) {
		manager.render(&lowresWindow);
	}
	else if (state == GameState::Pause) {
		manager.render(&lowresWindow);
		// TODO: render transparent cover + extra buttons (continue/menu)
	}

	lowresWindow.display();
	target->draw(lowresSprite, &manager.getShader());
	// ui
	manager.renderUI(target);

	// DEBUG FPS COUNTER
	target->draw(fpsText);
}
