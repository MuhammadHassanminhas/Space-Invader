#pragma once
#include <string>
#include <unordered_map>

class UserData {
public:
    // Load last saved level of player by name
    static unsigned short getLastLevel(const std::string& playerName);

    // Save player's current level to persistent storage
    static void savePlayerData(const std::string& playerName, unsigned short level);

private:
    // Internal storage filename
    static const std::string saveFilePath;

    // Load all player data from file into memory
    static std::unordered_map<std::string, unsigned short> loadAllData();

    // Save all player data from memory back to file
    static void saveAllData(const std::unordered_map<std::string, unsigned short>& data);
};
