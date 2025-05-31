
#include <array>
#include <chrono>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include "Headers/UserData.hpp"

#include "Headers/Animation.hpp"
#include "Headers/DrawText.hpp"
#include "Headers/Global.hpp"
#include "Headers/Enemy.hpp"
#include "Headers/EnemyManager.hpp"
#include "Headers/Ufo.hpp"
#include "Headers/Player.hpp"

int main() {
    bool game_over = false;
    bool next_level = false;
    bool restart_requested = false;
    bool player_initialized = false;
    bool show_splash = true;

    unsigned int lives = 3;
    unsigned short next_level_timer = NEXT_LEVEL_TRANSITION;

    std::string playerName;
    unsigned short level = 0;

    std::chrono::microseconds lag(0);
    std::chrono::steady_clock::time_point previous_time = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point splash_start_time = previous_time;

    std::mt19937_64 random_engine(std::chrono::system_clock::now().time_since_epoch().count());

    sf::Event event;

    sf::RenderWindow window(sf::VideoMode(SCREEN_RESIZE * SCREEN_WIDTH, SCREEN_RESIZE * SCREEN_HEIGHT), "Space Invaders", sf::Style::Close);
    window.setView(sf::View(sf::FloatRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)));

    sf::Texture background_texture, font_texture, powerup_bar_texture, logoTexture;
    background_texture.loadFromFile("Resources/Images/Background.png");
    font_texture.loadFromFile("Resources/Images/Font.png");
    powerup_bar_texture.loadFromFile("Resources/Images/PowerupBar.png");
    if (!logoTexture.loadFromFile("Resources/Images/logo.png"))
        return EXIT_FAILURE;

    sf::Sprite background_sprite(background_texture);
    sf::Sprite powerup_bar_sprite(powerup_bar_texture);
    sf::Sprite logoSprite(logoTexture);

    // Scale and center logo
    float scaleFactor = 0.2f;
    logoSprite.setScale(scaleFactor, scaleFactor);
    sf::Vector2u logoSize = logoTexture.getSize();
    logoSprite.setOrigin(logoSize.x / 2.0f, logoSize.y / 2.0f);
    logoSprite.setPosition(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);

    // Music
    sf::Music backgroundMusic;
    if (backgroundMusic.openFromFile("Resources/music/music.ogg")) {
        backgroundMusic.setLoop(true);
    }
    bool music_started = false;

    // Game objects
    EnemyManager enemy_manager;
    Player player;
    Ufo ufo(random_engine);

    while (window.isOpen()) {
        std::chrono::microseconds delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time);
        lag += delta_time;
        previous_time += delta_time;

        while (FRAME_DURATION <= lag) {
            lag -= FRAME_DURATION;

            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            if (show_splash) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                    std::cout << "Enter your username: ";
                    std::getline(std::cin, playerName);
                    level = UserData::getLastLevel(playerName);
                    show_splash = false;
                    player_initialized = true;
                    enemy_manager.reset(level);
                    player.reset();
                    ufo.reset(1, random_engine);
                }

                window.clear();
                window.draw(background_sprite);
                window.draw(logoSprite);
                window.display();
                continue;
            }

            if (!music_started && backgroundMusic.getStatus() != sf::Music::Playing) {
                backgroundMusic.play();
                music_started = true;
            }

            if (player.get_dead_animation_over()) {
                if (lives > 1) {
                    lives--;
                    player.reset();
                } else {
                    game_over = true;
                    UserData::savePlayerData(playerName, level);  // Save progress
                }
            }

            if (enemy_manager.reached_player(player.get_y())) {
                player.die();
            }

            if (!game_over) {
                if (enemy_manager.get_enemies().empty()) {
                    if (next_level_timer == 0) {
                        next_level = false;
                        level++;
                        next_level_timer = NEXT_LEVEL_TRANSITION;
                        player.reset();
                        enemy_manager.reset(level);
                        ufo.reset(1, random_engine);
                    } else {
                        next_level = true;
                        next_level_timer--;
                    }
                } else {
                    player.update(random_engine, enemy_manager.get_enemy_bullets(), enemy_manager.get_enemies(), ufo);
                    enemy_manager.update(random_engine);
                    ufo.update(random_engine);
                }
            }

            if (game_over && sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                if (!restart_requested) {
                    restart_requested = true;
                    game_over = false;
                    level = UserData::getLastLevel(playerName);
                    lives = 3;
                    player.reset();
                    player.resetScore();
                    enemy_manager.reset(level);
                    ufo.reset(1, random_engine);

                    if (music_started && backgroundMusic.getStatus() != sf::Music::Playing)
                        backgroundMusic.play();
                }
            } else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                restart_requested = false;
            }
        }

        if (FRAME_DURATION > lag) {
            window.clear();
            window.draw(background_sprite);

            if (show_splash) {
                window.draw(logoSprite);
            } else {
                if (!player.get_dead()) {
                    enemy_manager.draw(window);
                    ufo.draw(window);

                    if (player.get_current_power() > 0) {
                        powerup_bar_sprite.setColor(sf::Color::White);
                        powerup_bar_sprite.setPosition(SCREEN_WIDTH - powerup_bar_texture.getSize().x - 0.25f * BASE_SIZE, 0.25f * BASE_SIZE);
                        powerup_bar_sprite.setTextureRect(sf::IntRect(0, 0, powerup_bar_texture.getSize().x, BASE_SIZE));
                        window.draw(powerup_bar_sprite);

                        powerup_bar_sprite.setPosition(SCREEN_WIDTH - powerup_bar_texture.getSize().x - 0.125f * BASE_SIZE, 0.25f * BASE_SIZE);
                        powerup_bar_sprite.setTextureRect(sf::IntRect(0.125f * BASE_SIZE, BASE_SIZE,
                            ceil(player.get_power_timer() * static_cast<float>(powerup_bar_texture.getSize().x - 0.25f * BASE_SIZE) / POWERUP_DURATION), BASE_SIZE));

                        switch (player.get_current_power()) {
                            case 1: powerup_bar_sprite.setColor(sf::Color(0, 146, 255)); break;
                            case 2: powerup_bar_sprite.setColor(sf::Color(255, 0, 0)); break;
                            case 3: powerup_bar_sprite.setColor(sf::Color(255, 219, 0)); break;
                            case 4: powerup_bar_sprite.setColor(sf::Color(219, 0, 255)); break;
                        }

                        window.draw(powerup_bar_sprite);
                    }
                }

                player.draw(window);
                draw_text(0.25f * BASE_SIZE, 0.25f * BASE_SIZE, "Level: " + std::to_string(level), window, font_texture);
                // draw_text(0.25f * BASE_SIZE, 1.5f * BASE_SIZE, "Score: " + std::to_string(player.getScore()), window, font_texture);
                draw_text(0.25f * BASE_SIZE, 18.5f * BASE_SIZE, "Player: " + playerName, window, font_texture);

                if (game_over) {
                    draw_text(0.5f * (SCREEN_WIDTH - 5 * BASE_SIZE), 0.5f * (SCREEN_HEIGHT - BASE_SIZE), "Game over!", window, font_texture);
                    draw_text(0.5f * (SCREEN_WIDTH - 9.5f * BASE_SIZE), 0.5f * (SCREEN_HEIGHT + BASE_SIZE), "Press R to restart", window, font_texture);
                    // draw_text(0.5f * (SCREEN_WIDTH - 7 * BASE_SIZE), 0.5f * (SCREEN_HEIGHT + 3 * BASE_SIZE), "Final Score: " + std::to_string(player.getScore()), window, font_texture);
                } else if (next_level) {
                    draw_text(0.5f * (SCREEN_WIDTH - 5.5f * BASE_SIZE), 0.5f * (SCREEN_HEIGHT - BASE_SIZE), "Next level!", window, font_texture);
                }
            }

            window.display();
        }
    }
}
