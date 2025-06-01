#include "MonsterManager.hpp"


MonsterManager::MonsterManager(std::mt19937_64& rng) : rng(rng), spawnTimer(0.0f) {
    monsterTextures[0].loadFromFile("Resources/Images/monster1.png");
    monsterTextures[1].loadFromFile("Resources/Images/monster2.png");
    monsterTextures[2].loadFromFile("Resources/Images/monster3.png");
}

void MonsterManager::update(float deltaTime, std::vector<sf::Sprite>& enemyBullets, const sf::Texture& bulletTexture) {
    spawnTimer += deltaTime;
    if (spawnTimer >= 4.0f) {
        spawnMonster();
        spawnTimer = 0;
    }

    for (auto& monster : monsters) {
        monster.update(deltaTime);
        monster.shoot(enemyBullets, bulletTexture);
    }

    removeDeadMonsters();
}

void MonsterManager::draw(sf::RenderWindow& window) {
    for (auto& monster : monsters) {
        monster.draw(window);
    }
}

void MonsterManager::reset() {
    monsters.clear();
    spawnTimer = 0;
}

std::vector<Monster>& MonsterManager::getMonsters() {
    return monsters;
}

void MonsterManager::spawnMonster() {
    std::uniform_int_distribution<int> dist(0, 2);
    int type = dist(rng);
    float speed = 50.0f + type * 20.0f;
    monsters.emplace_back(monsterTextures[type], speed, type);
}

void MonsterManager::removeDeadMonsters() {
    monsters.erase(
        std::remove_if(monsters.begin(), monsters.end(), [](const Monster& m) {
            return m.isDead();
        }),
        monsters.end()
    );
}
