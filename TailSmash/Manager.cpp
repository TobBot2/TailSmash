#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include "Manager.h"

#include "Level.h"
#include "Player.h"

Manager::Manager(sf::Vector2f gameSize) 
	: gameSize(gameSize) {

	scoreText.setFillColor(sf::Color(0xe9efecff));
	scoreText.setOutlineColor(sf::Color(0x211e20ff));
	setScoreNormal();

	globalShader.loadFromFile("resources/globaleffects.frag", sf::Shader::Fragment);
	globalShader.setUniform("iResolution", gameSize);
	globalShader.setUniform("iTexture", sf::Shader::CurrentTexture);
	globalShader.setUniform("iShockPosition", sf::Vector2f(-9999, -9999));
	globalShader.setUniform("iShock", false);
	globalShader.setUniform("iFlash", false);

	expSml.loadFromFile("resources/exp-sml.wav");
	expMed.loadFromFile("resources/exp-med.wav");
	expBig.loadFromFile("resources/exp-big.wav");

	barrelSml.setBuffer(expSml);
	barrelMed.setBuffer(expMed);
	barrelBig.setBuffer(expBig);

#include "Levels.h"
	levels.push_back(lvl1);
	levels.push_back(lvl2);
	levels.push_back(lvl3);
	levels.push_back(lvl4);
	levels.push_back(lvl5);
}

void Manager::setPlayer(Player* player) {
	this->player = player;
	player->respawn(levels[activeLevel]->getSpawnPos(), levels[activeLevel]->getSpawnRot());
	player->setLevel(levels[activeLevel]);

	for (Level* l : levels) {
		l->setPlayer(player);
	}
}

void Manager::setFont(sf::Font& font) {
	scoreText = sf::Text("0.000", font);
}

void Manager::setScoreNormal() {
	scoreText.setOutlineThickness(scoreTextOutlineSize);
	scoreText.setCharacterSize(scoreTextSize);
	scoreText.setOrigin(scoreText.getGlobalBounds().width / 2.f, 0);
	scoreText.setPosition(gameSize.x / 2.f, 15.f);
}

void Manager::setScoreFinish() {
	scoreText.setOutlineThickness(scoreTextFinishOutlineSize);
	scoreText.setCharacterSize(scoreTextFinishSize);
	scoreText.setOrigin(scoreText.getGlobalBounds().getSize() / 2.f);
}

void Manager::resetLevel(int level) {
	if (level >= 0) {
		activeLevel = level;
	}

	levels[activeLevel]->reset();
	player->setLevel(levels[activeLevel]);
	setScoreNormal();
}

void Manager::update(float dt) {
	float sdt = dt * timeScale;
	player->update(sdt);
	levels[activeLevel]->update(sdt);
	globalTime += sdt;

	if (slomoTimer > 0.f) {
		slomoTimer -= dt;

		if (slomoTimer <= 0.f) {
			timeScale = 1.f;
		}
	}

	globalShader.setUniform("iTime", globalTime);

	std::ostringstream ss;
	ss << std::fixed << std::setprecision(3) << levels[activeLevel]->getScore();
	scoreText.setString(ss.str());
	if (levels[activeLevel]->isComplete() && waitAfterLevelTimer <= 0.f) {
		waitAfterLevelTimer = waitAfterLevelTime;
		player->disable();

		scoreText.setOrigin(scoreText.getGlobalBounds().getSize() / 2.f);
		setScoreFinish();
	}
	else if (waitAfterLevelTimer > 0.f) {
		waitAfterLevelTimer -= sdt;
		
		float t = (1 - waitAfterLevelTimer) / waitAfterLevelTime;
		float y = 1 * pow(2.7, -8 * t) * (1 - 8 * t) + gameSize.y / 4.f;
		scoreText.setPosition(gameSize.x / 2.f, y);

		if (waitAfterLevelTimer <= 0.f) {
			activeLevel++;
			activeLevel %= levels.size();

			resetLevel();
		}
	}
}

void Manager::render(sf::RenderTarget* target) const {
	levels[activeLevel]->render(target);
	player->render(target);
}

void Manager::renderUI(sf::RenderTarget* target) const {
	target->draw(scoreText);
}

void Manager::shockwave(sf::Vector2f position, float velSq) {
	globalShader.setUniform("iShockPosition", position);
	globalShader.setUniform("iShockStartTime", globalTime);

	bool shocking = velSq > 400.f;
	bool flashing = velSq > 1600.f;

	globalShader.setUniform("iShock", shocking);
	globalShader.setUniform("iFlash", flashing);

	barrelSml.play();

	// TODO: make the shocking/flashing modify the score too
	if (shocking) {
		slomoTimer = slomoTime;
		timeScale = slomoTimescale;
		barrelMed.play();
	}
	if (flashing) {
		slomoTimer *= 2;
		barrelBig.play();
	}
}
