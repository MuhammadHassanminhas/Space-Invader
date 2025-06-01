#include "Headers/UserData.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

const std::string UserData::saveFilePath = "player_data.txt";

// Load all player data (name -> {level, score})
std::unordered_map<std::string, std::pair<unsigned short, unsigned int>> UserData::loadAllData() {
    std::unordered_map<std::string, std::pair<unsigned short, unsigned int>> data;
    std::ifstream file(saveFilePath);
    if (!file.is_open()) {
        return data; // no save file
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string name;
        std::string levelStr, scoreStr;

        if (std::getline(iss, name, ',') &&
            std::getline(iss, levelStr, ',') &&
            std::getline(iss, scoreStr)) {
            
            unsigned short level = static_cast<unsigned short>(std::stoi(levelStr));
            unsigned int score = static_cast<unsigned int>(std::stoul(scoreStr));
            data[name] = { level, score };
        }
    }

    return data;
}

// Save all player data
void UserData::saveAllData(const std::unordered_map<std::string, std::pair<unsigned short, unsigned int>>& data) {
    std::ofstream file(saveFilePath, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Failed to open save file: " << saveFilePath << std::endl;
        return;
    }

    for (const auto& [name, pair] : data) {
        file << name << "," << pair.first << "," << pair.second << "\n";
    }
}

// Get last saved level of the player
unsigned short UserData::getLastLevel(const std::string& playerName) {
    auto data = loadAllData();
    auto it = data.find(playerName);
    if (it != data.end()) {
        return it->second.first;
    }
    return 0;
}

// Get last saved score of the player
unsigned int UserData::getPlayerScore(const std::string& playerName) {
    auto data = loadAllData();
    auto it = data.find(playerName);
    if (it != data.end()) {
        return it->second.second;
    }
    return 0;
}

// Save player level and score
void UserData::savePlayerData(const std::string& playerName, unsigned short level, unsigned int score) {
    auto data = loadAllData();
    data[playerName] = { level, score };
    saveAllData(data);
}
