#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include "Manager.h"

#include "Level.h"
#include "Player.h"

Manager::Manager(Player* player, sf::Vector2f gameSize, sf::Font& font) 
	: player(player), gameSize(gameSize) {

	scoreText = sf::Text("0.000", font);
	scoreText.setOutlineColor(sf::Color::Black); // infill color is white by defualt
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

	// LEVEL 1
	Level* lvl1 = new Level(player);
	lvl1->setSpawn(sf::Vector2f(gameSize.x * .2f, gameSize.y * .8f), 90.f);
	lvl1->addTarget(sf::Vector2f(gameSize.x * .2f, gameSize.y * .5f));
	lvl1->addTarget(sf::Vector2f(gameSize.x * .5f, gameSize.y * .5f));
	lvl1->addTarget(sf::Vector2f(gameSize.x * .8f, gameSize.y * .5f));
	lvl1->addWall(sf::Vector2f(0.f, 0.f), sf::Vector2f(gameSize.x, gameSize.y * .1f)); // horizontal border
	lvl1->addWall(sf::Vector2f(0.f, gameSize.y * .9f), sf::Vector2f(gameSize.x, gameSize.y * .1f)); // horizontal border
	lvl1->addWall(sf::Vector2f(0.f, gameSize.y * .105f), sf::Vector2f(gameSize.y * .1f, gameSize.y * .79f)); // vertical border
	lvl1->addWall(sf::Vector2f(gameSize.x - gameSize.y * .1f, gameSize.y * .105f), sf::Vector2f(gameSize.y * .1f, gameSize.y * .79f)); // vertical border

	// LEVEL 2
	Level* lvl2 = new Level(player);
	lvl2->setSpawn(sf::Vector2f(gameSize.x * .8f, gameSize.y * .2f), 0.f);
	lvl2->addTarget(sf::Vector2f(gameSize.x * .2f, gameSize.y * .2f));
	lvl2->addTarget(sf::Vector2f(gameSize.x * .2f, gameSize.y * .8f));
	lvl2->addTarget(sf::Vector2f(gameSize.x * .3f, gameSize.y * .5f));
	lvl2->addWall(sf::Vector2f(gameSize.x * .5f, gameSize.y * .45f), sf::Vector2f(gameSize.x * .5f, gameSize.y * .1f));
	lvl2->addWall(sf::Vector2f(0.f, 0.f), sf::Vector2f(gameSize.x, gameSize.y * .1f)); // horizontal border
	lvl2->addWall(sf::Vector2f(0.f, gameSize.y * .9f), sf::Vector2f(gameSize.x, gameSize.y * .1f)); // horizontal border
	lvl2->addWall(sf::Vector2f(0.f, gameSize.y * .11f), sf::Vector2f(gameSize.y * .1f, gameSize.y * .78f)); // vertical border
	lvl2->addWall(sf::Vector2f(gameSize.x - gameSize.y * .1f, gameSize.y * .11f), sf::Vector2f(gameSize.y * .1f, gameSize.y * .78f)); // vertical border

	levels.push_back(lvl1);
	levels.push_back(lvl2);

	player->respawn(lvl1->getSpawnPos(), lvl1->getSpawnRot());
	player->setLevel(lvl1);
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

			levels[activeLevel]->reset();
			player->setLevel(levels[activeLevel]);
			setScoreNormal();
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
