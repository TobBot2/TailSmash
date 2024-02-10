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
	levels.push_back(tailsmash);
	levels.push_back(dots);
	levels.push_back(level2);
	levels.push_back(lvl12punch);
	levels.push_back(nuisance);
	levels.push_back(lvlAbstract);
	levels.push_back(rounder);
	levels.push_back(fractals);
	levels.push_back(trenches);
	levels.push_back(detour);
	levels.push_back(pillars);
	levels.push_back(theplunge);
	levels.push_back(bricks);
	levels.push_back(cavern);
	levels.push_back(labyrinth);
	levels.push_back(theface);
	levels.push_back(initiate);
	levels.push_back(warpspeed);
	levels.push_back(asteroids);
	levels.push_back(spaceship);
	levels.push_back(inside);
	levels.push_back(diamond);
	levels.push_back(cleanup);
	levels.push_back(spire);
	levels.push_back(fin);
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

void Manager::setVolume(float volume){
	barrelSml.setVolume(volume * .7f); // icky 'manually' lowering volume of the .wav file
	barrelMed.setVolume(volume);
	barrelBig.setVolume(volume);
}

void Manager::resetLevel(int level) {
	if (level >= 0) {
		activeLevel = level;
	}

	levels[activeLevel]->reset();
	player->setLevel(levels[activeLevel]);
	setScoreNormal();
	waitAfterLevelTimer = 0.f;
}

Level* Manager::getLevel(int i) {
	if (i == -1) return levels[activeLevel];
	return levels[i];
}

int Manager::getLevelCount() {
	return levels.size();
}

std::vector<float> Manager::getScores() {
	std::vector<float> scores;
	for (int i = 0; i < levels.size(); i++) {
		float score = levels[i]->getHighScore();
		scores.push_back(score);
	}
	return scores;
}

void Manager::setScores(std::vector<float> scores) {
	for (int i = 0; i < levels.size(); i++) {
		levels[i]->setHighScore(scores[i]);
	}
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
