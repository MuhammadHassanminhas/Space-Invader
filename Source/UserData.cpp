#include "Headers/UserData.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

const std::string UserData::saveFilePath = "player_data.txt";

std::unordered_map<std::string, unsigned short> UserData::loadAllData() {
    std::unordered_map<std::string, unsigned short> data;
    std::ifstream file(saveFilePath);
    if (!file.is_open()) {
        // No save file yet, return empty data
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string name;
        unsigned short level;

        if (std::getline(iss, name, ',') && iss >> level) {
            data[name] = level;
        }
    }
    return data;
}

void UserData::saveAllData(const std::unordered_map<std::string, unsigned short>& data) {
    std::ofstream file(saveFilePath, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Failed to open save file for writing: " << saveFilePath << std::endl;
        return;
    }

    for (const auto& [name, level] : data) {
        file << name << "," << level << "\n";
    }
}

unsigned short UserData::getLastLevel(const std::string& playerName) {
    auto data = loadAllData();
    auto it = data.find(playerName);
    if (it != data.end()) {
        return it->second;
    }
    return 0; // default level if not found
}

void UserData::savePlayerData(const std::string& playerName, unsigned short level) {
    auto data = loadAllData();
    data[playerName] = level;
    saveAllData(data);
}
