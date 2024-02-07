#include <iostream>
#include <SFML/Audio.hpp>

#include "imgui.h"
#include "imgui-SFML.h"

#include "Game.h"

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
	music.setVolume(musicVolume);

	fpsText = sf::Text("", font);
	fpsText.setFillColor(sf::Color(0xe9efecff));
	fpsText.setOutlineColor(sf::Color(0x211e20ff));
	fpsText.setOutlineThickness(3.f);
	fpsText.setPosition(0, 0);

	manager.setFont(font);
	manager.setPlayer(&player);
	manager.setVolume(sfxVolume);
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

void Game::render(sf::RenderTarget* target) {
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
		ImVec2 screenSize((float)target->getSize().x, (float)target->getSize().y);
		ImVec2 buttonSize(screenSize.x * .3f, screenSize.y * .2f); // Size of the ImGui window
		ImVec2 windowPos((screenSize.x - buttonSize.x) / 2.f, (screenSize.y - buttonSize.y) / 2.f);

		ImGui::SetNextWindowPos(windowPos);
		ImGui::Begin("main buttons", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
		if (ImGui::Button("Play", buttonSize)) {
			state = GameState::Play;
			manager.setScoreNormal();
		}
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), buttonSize.y * 1.15f));
		if (ImGui::Button("Levels", ImVec2(buttonSize.x * .5f, buttonSize.y * .5f))) {
			state = GameState::LevelSelect;
		}
		ImGui::SetCursorPos(ImVec2(buttonSize.x * .6f, buttonSize.y * 1.15f));
		if (ImGui::Button("Quit", ImVec2(buttonSize.x * .4f, buttonSize.y * .4f))) {
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
		ImVec2 gridSize(target->getSize().x * .6f, target->getSize().y * .9f);
		ImVec2 gridPos(target->getSize().x * .2f, target->getSize().y * .05f);
		ImGui::SetNextWindowSize(gridSize);
		ImGui::SetNextWindowPos(gridPos);
		ImGui::Begin("level buttons", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);

		ImVec2 buttonSize(ImGui::GetWindowSize().x / 8.f, ImGui::GetWindowSize().x / 10.f);
		const int GRID_SIZE = 5;
		// Display grid of buttons
		for (int i = 0; i < GRID_SIZE; ++i) {
			ImGui::Columns(GRID_SIZE, nullptr, true);
			ImGui::Separator();
			for (int j = 0; j < GRID_SIZE; ++j) {
				int level = i * GRID_SIZE + j;
				if (ImGui::Button((std::to_string(level + 1)).c_str(), buttonSize)) {
					manager.resetLevel(level);
					state = GameState::Play;
				}
				ImGui::NextColumn();
			}
			ImGui::Separator();
		}
		ImGui::End();

		ImVec2 backButtonSize(target->getSize().x * .4f, buttonSize.y * 1.5f);
		ImGui::SetNextWindowPos(ImVec2(gridPos.x, gridPos.y + gridSize.y - buttonSize.y * 2.5f));
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

		ImGui::TextWrapped("Font - Mont (demo). https://www.dafont.com/mont.font");
		ImGui::Text("");
		ImGui::TextWrapped("Music - Ludem Dare 38 - Track 9. https://soundcloud.com/abstraction/ludum-dare-38-track-nine-game-loop-free-download?in=abstraction/sets/ludum-dare-challenge&utm_source=clipboard&utm_medium=text&utm_campaign=social_sharing");
		ImGui::Text("");
		ImGui::TextWrapped("Explosion sound effects:");
		ImGui::TextWrapped(" - https://freesound.org/people/dobroide/sounds/24635/");
		ImGui::TextWrapped(" - https://freesound.org/people/LiamG_SFX/sounds/322502/");
		ImGui::TextWrapped(" - https://freesound.org/people/LiamG_SFX/sounds/322509/");
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
