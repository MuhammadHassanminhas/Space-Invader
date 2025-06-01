#pragma once
#ifndef USER_DATA_HPP
#define USER_DATA_HPP

#include <string>
#include <unordered_map>

class UserData {
public:
    static unsigned short getLastLevel(const std::string& playerName);
    static unsigned int getPlayerScore(const std::string& playerName);
    static void savePlayerData(const std::string& playerName, unsigned short level, unsigned int score);

private:
    static std::unordered_map<std::string, std::pair<unsigned short, unsigned int>> loadAllData();
    static void saveAllData(const std::unordered_map<std::string, std::pair<unsigned short, unsigned int>>& data);
    static const std::string saveFilePath;
};

#endif
