#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <SFML/Audio.hpp>

#include "imgui.h"
#include "imgui-SFML.h"

#include "Game.h"
#include "Level.h"

Game::Game(sf::View view) :
	player(sf::Vector2f(view.getSize()) / 2.f, sf::Vector2f(60.f, 35.f)),
	manager(sf::Vector2f(view.getSize())) {
	
	this->view = view;
	lowresWindow.create(view.getSize().x / pixelSize, view.getSize().y / pixelSize);
	lowresWindow.setSmooth(false);

	lowresSprite = sf::Sprite(lowresWindow.getTexture());
	lowresSprite.setScale(sf::Vector2f(pixelSize, pixelSize));

	if (!font.loadFromFile("resources/mont.otf")) {
		std::cout << "error loading font resources/mont.otf\n";
	}

	if (!music.openFromFile("resources/musicLD38T9.wav")) {
		std::cout << "error loading background music resources/musicLD38T9.wav\n";
	}
	music.setLoop(true);

	fpsText = sf::Text("", font);
	fpsText.setFillColor(sf::Color(0xe9efecff));
	fpsText.setOutlineColor(sf::Color(0x211e20ff));
	fpsText.setOutlineThickness(3.f);
	fpsText.setPosition(0, 0);

	manager.setFont(font);
	manager.setPlayer(&player);
	player.setManager(&manager);


	// load settings
	std::ifstream settingsIn;
	settingsIn.open("resources/settings.bin", std::ios::binary);
	settingsIn.read((char*)&musicVolume, sizeof(float));
	if (settingsIn.gcount() != sizeof(float)) {
		musicVolume = 25; // DEFUAULT VALUE
	}
	else {
		settingsIn.read((char*)&sfxVolume, sizeof(float));
		if (settingsIn.gcount() != sizeof(float)) {
			sfxVolume = 25;
		}
	}
	settingsIn.close();
	music.setVolume(musicVolume);
	manager.setVolume(sfxVolume);

	// load highscores
	std::ifstream highscoresIn;
	highscoresIn.open("resources/highscores.bin", std::ios::binary);
	while (!highscoresIn.eof()) {
		float score = -1.f;
		highscoresIn.read((char*)&score, sizeof(float));
		scores.push_back(score);
	}
	scores.pop_back(); // reads one too many... just pop the last one
	highscoresIn.close();
	if (scores.size() != manager.getLevelCount()) {
		std::ofstream highscoresOut;
		highscoresOut.open("resources/highscores.bin", std::ios::out | std::ios::trunc | std::ios::binary);
		float uninitialized = -1.f;
		if (scores.size() == 5) {
			// old highscore data. Convert...
			std::vector<float> newScores = {};
			for (int i = 0; i < manager.getLevelCount(); i++) {
				switch (i) {
				case 1:
					highscoresOut.write((char*)&scores[0], sizeof(float));
					newScores.push_back(scores[0]);
					break;
				case 2:
					highscoresOut.write((char*)&scores[1], sizeof(float));
					newScores.push_back(scores[1]);
					break;
				case 3:
					highscoresOut.write((char*)&scores[2], sizeof(float));
					newScores.push_back(scores[2]);
					break;
				case 5:
					highscoresOut.write((char*)&scores[3], sizeof(float));
					newScores.push_back(scores[3]);
					break;
				case 6:
					highscoresOut.write((char*)&scores[4], sizeof(float));
					newScores.push_back(scores[4]);
					break;
				default:
					highscoresOut.write((char*)&uninitialized, sizeof(float));
					newScores.push_back(-1);
					break;
				}
			}
			scores = newScores;
			manager.setScores(scores);
		}
		else {
			// highscores.bin has not been initialized. seed with -1's...
			for (int i = 0; i < manager.getLevelCount(); i++) {
				highscoresOut.write((char*)&uninitialized, sizeof(float));
			}
		}
		netHighScore = -1.f;
		highscoresOut.close();
	}
	else {
		manager.setScores(scores);
	}
}

void Game::update(float elapsedTime) {
	updateFpsTextTimer -= elapsedTime;
	if (updateFpsTextTimer < 0) {
		fpsText.setString("framerate: " + std::to_string((int)(1.f / elapsedTime)));
		updateFpsTextTimer = updateFpsTextTime;
	}

	if (state == GameState::Play) {
		manager.update(elapsedTime);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
			state = GameState::Pause;
		}
	}
	else if (state == GameState::Splash) {
		splashDelay -= elapsedTime;
		if (splashDelay <= 0) {
			state = GameState::Menu;
			music.play();
		}
	}
}

void Game::render(sf::RenderWindow* target) {
	lowresWindow.setView(view);

	// render
	lowresWindow.clear(sf::Color(0x555568ee));

	if (state == GameState::Play) {
		manager.render(&lowresWindow);
		lowresWindow.display();
		target->draw(lowresSprite, &manager.getShader());
		manager.renderUI(target);
	}
	else if (state == GameState::Pause) {
		manager.render(&lowresWindow);
		ImVec2 screenSize((float)target->getSize().x, (float)target->getSize().y);
		ImVec2 buttonSize(screenSize.x * .3f, screenSize.y * .2f); // Size of the ImGui window
		ImVec2 windowPos((screenSize.x - buttonSize.x) / 2.f, (screenSize.y - buttonSize.y) / 2.f);

		ImGui::SetNextWindowPos(windowPos);
		ImGui::Begin("no window", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
		if (ImGui::Button("Resume", buttonSize)) {
			state = GameState::Play;
		}
		ImGui::SetCursorPos(ImVec2(buttonSize.x / 4.f, buttonSize.y * 1.15f));
		if (ImGui::Button("Menu", ImVec2(buttonSize.x * .5f, buttonSize.y * .5f))) {
			state = GameState::Menu;
		}
		ImGui::End();

		lowresWindow.display();
		target->draw(lowresSprite, &manager.getShader());
		manager.renderUI(target);
	}
	else if (state == GameState::Menu) {
		scores = manager.getScores();
		netHighScore = 0.f;
		for (int i = 0; i < scores.size(); i++) {
			if (scores[i] < 0) {
				netHighScore = -1.f;
				break;
			}
			netHighScore += scores[i];
		}

		float highscoreOffset = 0.f;
		if (netHighScore >= 0) {
			highscoreOffset = ImGui::GetFontSize();
		}

		ImVec2 screenSize((float)target->getSize().x, (float)target->getSize().y);
		ImVec2 buttonSize(screenSize.x * .3f, screenSize.y * .2f); // Size of the ImGui window
		ImVec2 windowPos((screenSize.x - buttonSize.x) / 2.f, (screenSize.y - buttonSize.y) / 2.f - highscoreOffset);

		ImGui::SetNextWindowPos(windowPos);
		ImGui::Begin("main buttons", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
		
		if (netHighScore >= 0) {
			std::ostringstream ss;
			ss << "Highscore: " << std::fixed << std::setprecision(3) << netHighScore;
			ImGui::Text(ss.str().c_str());
		}
		if (ImGui::Button("Play", buttonSize)) {
			state = GameState::Play;
			manager.setScoreNormal();
		}

		float bottomButtonsY = ImGui::GetCursorPosY() + buttonSize.y * .15f;
		ImGui::SetCursorPosY(bottomButtonsY);
		if (ImGui::Button("Levels", ImVec2(buttonSize.x * .5f, buttonSize.y * .5f))) {
			state = GameState::LevelSelect;
		}
		ImGui::SetCursorPos(ImVec2(buttonSize.x * .6f, bottomButtonsY));
		if (ImGui::Button("Quit", ImVec2(buttonSize.x * .4f, buttonSize.y * .4f))) {
			saveScores();
			ImGui::SFML::Shutdown();
			exit(0); // unclean exit but idc
		}
		ImGui::End();

		// settings/credits
		ImGui::SetNextWindowPos(ImVec2(target->getSize().x - buttonSize.x * 1.1f, target->getSize().y - buttonSize.y * .55f));
		ImGui::Begin("settings/credits", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
		if (ImGui::Button("Settings", ImVec2(buttonSize.x * .48f, buttonSize.y * .4f))) {
			state = GameState::Settings;
		}
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		if (ImGui::Button("Credits", ImVec2(buttonSize.x * .48f, buttonSize.y * .4f))) {
			state = GameState::Credits;
		}
		ImGui::End();

		lowresWindow.display();
		target->draw(lowresSprite);
	}
	else if (state == GameState::LevelSelect) {
		ImVec2 levelSelectSize(target->getSize().x * .6f, target->getSize().y * .9f);
		ImVec2 levelSelectPos(target->getSize().x * .2f, target->getSize().y * .05f);
		ImGui::SetNextWindowSize(levelSelectSize);
		ImGui::SetNextWindowPos(levelSelectPos);
		ImGui::Begin("level buttons", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);

		ImVec2 buttonSize(ImGui::GetWindowSize().x * .6f, ImGui::GetFontSize() * 2.f);
		const int GRID_SIZE = 5;
		// Display grid of buttons
		for (int i = 0; i < manager.getLevelCount(); i++) {
			// play button (w/ title)
			if (ImGui::Button(manager.getLevel(i)->getName().c_str(), buttonSize)) {
				manager.resetLevel(i);
				state = GameState::Play;
			}

			// highscore
			ImGui::SameLine();
			if (i < manager.getLevelCount() && manager.getLevel(i)->getHighScore() >= 0) {
				std::ostringstream ss;
				ss << "Time: " << std::fixed << std::setprecision(3) << manager.getLevel(i)->getHighScore();
				ImGui::Text(ss.str().c_str());
			}
			else {
				ImGui::Text("No Time");
			}

			ImGui::Text(""); // spacing
		}
		ImGui::End();

		ImVec2 backButtonSize(levelSelectPos.x - 30, buttonSize.y * 1.5f);
		ImGui::SetNextWindowPos(ImVec2(15, 15));
		ImGui::Begin("back button", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
		// Exit button separate from the grid
		if (ImGui::Button("Back", backButtonSize)) {
			state = GameState::Menu;
		}

		ImGui::End();

		lowresWindow.display();
		target->draw(lowresSprite);
	}
	else if (state == GameState::Settings) {
		ImVec2 screenSize(target->getSize().x * .8f, target->getSize().y * .8f);
		ImGui::SetNextWindowSize(screenSize);
		ImGui::SetNextWindowPos(ImVec2((target->getSize().x - screenSize.x) / 2.f, (target->getSize().y - screenSize.y) / 2.f));
		ImGui::Begin("settings", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
		
		ImGui::SliderFloat("Music Volume", &musicVolume, 0.f, 100.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
		music.setVolume(musicVolume);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.f);
		ImGui::SliderFloat("SFX Volume", &sfxVolume, 0.f, 100.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
		manager.setVolume(sfxVolume);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.f);
		if (ImGui::Button("Reset Scores")) {
			scores = {};
			for (int i = 0; i < manager.getLevelCount(); i++) {
				scores.push_back(-1.f);
			}
			manager.setScores(scores);
		}

		std::ofstream settingsOut;
		settingsOut.open("resources/settings.bin", std::ios::out | std::ios::trunc | std::ios::binary);
		settingsOut.write((char*)&musicVolume, sizeof(float));
		settingsOut.write((char*)&sfxVolume, sizeof(float));
		settingsOut.close();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.f);
		if (ImGui::Button("Back", ImVec2(screenSize.x / 10.f, screenSize.y / 11.f))) {
			state = GameState::Menu;
		}

		ImGui::End();

		lowresWindow.display();
		target->draw(lowresSprite);
	}
	else if (state == GameState::Credits) {
		ImVec2 creditsSize(target->getSize().x * .7f, target->getSize().y * .8f);
		ImGui::SetNextWindowPos(ImVec2((target->getSize().x - creditsSize.x) / 2.f, target->getSize().y * .1f));
		ImGui::SetNextWindowSize(creditsSize);
		ImGui::Begin("credits", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);

		ImGui::TextWrapped("Font - Mont (demo). www.dafont.com/mont.font");
		ImGui::Text("");
		ImGui::TextWrapped("Music - Ludem Dare 38 - Track 9. soundcloud.com/abstraction/ludum-dare-38-track-nine-game-loop-free-download?in=abstraction/sets/ludum-dare-challenge&utm_source=clipboard&utm_medium=text&utm_campaign=social_sharing");
		ImGui::Text("");
		ImGui::TextWrapped("Explosion sound effects:");
		ImGui::TextWrapped(" - freesound.org/people/dobroide/sounds/24635/");
		ImGui::TextWrapped(" - freesound.org/people/LiamG_SFX/sounds/322502/");
		ImGui::TextWrapped(" - freesound.org/people/LiamG_SFX/sounds/322509/");
		ImGui::Text("");
		ImGui::TextWrapped("Shaders:");
		ImGui::TextWrapped(" - shadertoy.com/view/XtlSD7");
		ImGui::TextWrapped(" - shadertoy.com/view/lldGzr");
		

		ImGui::SetCursorPos(ImVec2((creditsSize.x - creditsSize.x / 2.5f) / 2.f, ImGui::GetCursorPos().y + 40));
		if (ImGui::Button("Back", ImVec2(creditsSize.x / 2.5f, creditsSize.x / 7.f))) {
			state = GameState::Menu;
		}

		ImGui::End();

		lowresWindow.display();
		target->draw(lowresSprite);
	}
	else if (state == GameState::Splash) {
		ImVec2 imageSize(ImGui::CalcTextSize("IT Co."));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("splash page", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
		float fadeTime = 2.5f;
		int alpha = splashDelay > 3.f - fadeTime ? (3.f - splashDelay) / fadeTime * 255 : 255;
		ImGui::SetCursorPos(ImVec2((target->getSize().x - imageSize.x) / 2.f, (target->getSize().y - imageSize.y) / 2.f));
		ImGui::TextColored(ImVec4(sf::Color(255, 255, 255, alpha)), "IT Co.");
		//ImGui::Image(splashImg, sf::Color(255, 255, 255, alpha));
		ImGui::End();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
			state = GameState::Menu;
			music.play();
		}

		lowresWindow.display();
		target->draw(lowresSprite);
	}

	// DEBUG FPS COUNTER
	//target->draw(fpsText);
}

void Game::saveScores() {
	std::ofstream highscoresOut;
	highscoresOut.open("resources/highscores.bin", std::ios::out | std::ios::trunc | std::ios::binary);
	for (int i = 0; i < scores.size(); i++) {
		highscoresOut.write((char*) &scores[i], sizeof(float));
	}
	highscoresOut.close();
}
